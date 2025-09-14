#include "FreeList.h"
#include <corecrt_malloc.h>
#include <cassert>


NodeForFreeList::NodeForFreeList(const size_t& dataToSort, const void* ptr) : TNode(dataToSort, (void*)ptr) {
}


NodeForFreeList* NodeForFreeList::getSibling(const NodeForFreeList* node) const {
	return childs[!getNodePosition(node)];
}

bool NodeForFreeList::wouldJoin() const {
	return  listNodeIsFree(LEFT) || listNodeIsFree(RIGHT);
}

bool NodeForFreeList::listNodeIsFree(NodePosition position) const
{
	return listNode[position] != nullptr && listNode[position]->isFree;
}

void NodeForFreeList::join(Direction direction) {
	if (!listNodeIsFree(direction)) return;
	NodeForFreeList* nodeToDelete = listNode[direction];
	if (direction == LEFT) _dataTuple = nodeToDelete->_dataTuple;
	_dataToSort += nodeToDelete->_dataToSort;
	listNode[direction] = nodeToDelete->listNode[direction];
	if (listNode[direction] != nullptr)
		listNode[direction]->listNode[!direction] = this;
	delete nodeToDelete;
}

void NodeForFreeList::split(NodeForFreeList* node) {
	node->_dataTuple = (char*)std::get<void*>(_dataTuple) + _dataToSort;
	node->listNode[LEFT] = this;
	node->listNode[RIGHT] = listNode[RIGHT];
	listNode[RIGHT] = node;
}



void BlackRedTreeForFreeList::split(NodeForFreeList* node, size_t size) {
	size_t newSize = node->getData() - size;
	if (newSize <= MIN_SPLIT) return;

	node->setData(size);

	NodeForFreeList* newNode = new NodeForFreeList(newSize, 0);
	node->split(newNode);
	insertFreeNode(newNode);
}

void BlackRedTreeForFreeList::insertFreeNode(NodeForFreeList* node) {
	NodeForFreeList* currentNode = _root;
	NodePosition currentNodePosition = LEFT;

	//binary inserction
	while (currentNode != nullptr) {
		node->parent = currentNode;
		currentNodePosition = (NodePosition)(*node >= currentNode);
		currentNode = currentNode->childs[currentNodePosition];
	}


	if (node->parent == nullptr) {
		_root = node;
		_root->color = BLACK;
		return;
	}

	node->parent->childs[currentNodePosition] = node;

	if (node->parent->color == RED) {
		fixInsertViolation(node);
		return;
	}

	if (_root == nullptr) _root = node;
}

void BlackRedTreeForFreeList::fixInsertViolation(NodeForFreeList* node) {
	assert(nullptr != node);

	if (node->parent == nullptr) return;

	NodeForFreeList* currentNode = node;

	while (currentNode != _root && currentNode->parent->color == RED) {
		NodeForFreeList* grandparent = currentNode->parent->parent;
		NodeForFreeList* uncle = grandparent->getSibling(currentNode->parent);
		NodePosition parentPosition = grandparent->getNodePosition(currentNode->parent);
		NodePosition oppositeParentPosition = (NodePosition)(parentPosition == LEFT);

		if (uncle == nullptr || uncle->color == BLACK) {
			if (currentNode->parent->getNodePosition(currentNode) == oppositeParentPosition) {
				rotation(currentNode->parent, (BRTRotationDirection)parentPosition);
			}
			currentNode->parent->color = BLACK;
			grandparent->color = RED;
			rotation(grandparent, (BRTRotationDirection)oppositeParentPosition);
			continue;
		}

		currentNode->parent->color = BLACK;
		uncle->color = BLACK;
		grandparent->color = RED;
		currentNode = grandparent;
	}
	_root->color = BLACK;
}

void BlackRedTreeForFreeList::fixRemoveViolation(NodeForFreeList* node) {
	assert(nullptr != node);

	NodeForFreeList* currentNode = node;
	while (currentNode != nullptr && currentNode->parent != nullptr && currentNode->color == BLACK) {
		NodeForFreeList* curretNodeSibling = currentNode->parent->getSibling(currentNode);
		NodePosition currentNodePosition = currentNode->parent->getNodePosition(currentNode);
		NodePosition oppositePosition = (NodePosition)(currentNodePosition == LEFT);

		// Fix: handle nullptr sibling as black
		if (curretNodeSibling == nullptr) {
			currentNode = currentNode->parent;
			continue;
		}

		if (curretNodeSibling->color == RED) {
			curretNodeSibling->color = BLACK;
			currentNode->parent->color = RED;
			rotation(currentNode->parent, currentNodePosition);
			curretNodeSibling = currentNode->parent->childs[oppositePosition];
			if (curretNodeSibling == nullptr) {
				currentNode = currentNode->parent;
				continue;
			}
		}

		bool isLeftChildBlack = curretNodeSibling->childs[LEFT] == nullptr || curretNodeSibling->childs[LEFT]->color == BLACK;
		bool isRightChildBlack = curretNodeSibling->childs[RIGHT] == nullptr || curretNodeSibling->childs[RIGHT]->color == BLACK;
		if (curretNodeSibling->howManyChilds() == 0 || (isLeftChildBlack && isRightChildBlack)) {
			curretNodeSibling->color = RED;
			currentNode = currentNode->parent;
			continue;
		}

		if (curretNodeSibling->childs[oppositePosition] == nullptr ||
			curretNodeSibling->childs[oppositePosition]->color == BLACK) {

			if (curretNodeSibling->childs[currentNodePosition] != nullptr)
				curretNodeSibling->childs[currentNodePosition]->color = BLACK;

			curretNodeSibling->color = RED;
			rotation(curretNodeSibling, oppositePosition);
			curretNodeSibling = currentNode->parent->childs[oppositePosition];
			if (curretNodeSibling == nullptr) {
				currentNode = currentNode->parent;
				continue;
			}
		}

		curretNodeSibling->color = currentNode->parent->color;
		currentNode->parent->color = BLACK;
		if (curretNodeSibling->childs[oppositePosition] != nullptr)
			curretNodeSibling->childs[oppositePosition]->color = BLACK;
		rotation(currentNode->parent, currentNodePosition);
		currentNode = _root;
	}
	if (currentNode != nullptr) currentNode->color = BLACK;
}

void BlackRedTreeForFreeList::rotation(NodeForFreeList* node, const BRTRotationDirection rotation) {
	const BRTRotationDirection oppositeRotation = (NodePosition)(rotation == LEFT);

	assert(node != nullptr);
	assert(node->childs[oppositeRotation] != nullptr);

	NodeForFreeList* secondNode = node->childs[oppositeRotation];

	// Move secondNode's child to node
	node->childs[oppositeRotation] = secondNode->childs[rotation];
	if (secondNode->childs[rotation] != nullptr)
		secondNode->childs[rotation]->parent = node;

	// Update parent pointers
	secondNode->parent = node->parent;
	if (node->parent == nullptr) {
		_root = secondNode;
	}
	else {
		NodePosition nodePosition = node->parent->getNodePosition(node);
		node->parent->childs[nodePosition] = secondNode;
	}

	node->parent = secondNode;
	secondNode->childs[rotation] = node;
	secondNode->childs[rotation] = node;
}

void BlackRedTreeForFreeList::normalRemove(NodeForFreeList* node) {
	NodeForFreeList* replaceNode = node;
	NodePosition nodePosition;
	NodePosition currentNodePosition;

	//BST remove
	switch (node->howManyChilds()) {
	case 2:
		replaceNode = node->childs[RIGHT];
		while (replaceNode->childs[LEFT] != nullptr) replaceNode = replaceNode->childs[LEFT];

		currentNodePosition = (NodePosition)(replaceNode->parent == node);
		if (replaceNode->childs[RIGHT] != nullptr) {
			replaceNode->parent->childs[currentNodePosition] = replaceNode->childs[RIGHT];
			replaceNode->childs[RIGHT]->parent = replaceNode->parent;
		}

		node->childs[LEFT]->parent = replaceNode;
		node->childs[RIGHT]->parent = replaceNode;
		replaceNode->childs[LEFT] = node->childs[LEFT] != replaceNode ? node->childs[LEFT] : nullptr;
		replaceNode->childs[RIGHT] = node->childs[RIGHT] != replaceNode ? node->childs[RIGHT] : nullptr;

		replaceNode->parent = node->parent;
		if (node->parent == nullptr) {
			_root = replaceNode;
			break;
		}

		node->parent->childs[node->parent->getNodePosition(node)] = replaceNode;
		break;
	case 1:

		replaceNode = node->childs[node->childs[LEFT] == nullptr];
		replaceNode->parent = node->parent;

		if (node->parent == nullptr) {
			_root = replaceNode;
			break;
		}

		nodePosition = node->parent->getNodePosition(node);
		node->parent->childs[nodePosition] = replaceNode;
		break;
	default:
		if (node->parent == nullptr) {
			_root = nullptr;
			break;
		}
		nodePosition = node->parent->getNodePosition(node);
		node->parent->childs[nodePosition] = nullptr;
		break;
	}

	if (node->color == BLACK) fixRemoveViolation(replaceNode);
}

BlackRedTreeForFreeList::BlackRedTreeForFreeList(const void* startPtr, const size_t& size) {
	_root = new NodeForFreeList(size, startPtr);
	_root->color = BLACK;
}

NodeForFreeList* BlackRedTreeForFreeList::insert(const size_t& size) {
	NodeForFreeList* currentNode = _root;
	NodePosition currentNodePosition = LEFT;

	//binary inserction/search

	while (currentNode != nullptr && !(*currentNode >= size && currentNode->isFree)) {
		currentNodePosition = (NodePosition)(*currentNode >= size);
		currentNode = currentNode->childs[currentNodePosition];
	}

	if (currentNode == nullptr) {
		return nullptr;
	}

	split(currentNode, size);

	currentNode->isFree = false;

}

bool BlackRedTreeForFreeList::remove(NodeForFreeList* nodeToRemove) {
	if (nodeToRemove == nullptr) return false;

	nodeToRemove->isFree = true;


	return true;

	if (!nodeToRemove->wouldJoin()) {
		return true;
	}

	normalRemove(nodeToRemove);

	if (nodeToRemove->listNodeIsFree(LEFT)) {
		normalRemove(nodeToRemove->listNode[LEFT]);
		nodeToRemove->join(LEFT);
	}
	if (nodeToRemove->listNodeIsFree(RIGHT)) {
		normalRemove(nodeToRemove->listNode[RIGHT]);
		nodeToRemove->join(RIGHT);
	}
	nodeToRemove->childs[LEFT] = nullptr;
	nodeToRemove->childs[RIGHT] = nullptr;
	nodeToRemove->color = RED;

	insertFreeNode(nodeToRemove);

	return true;
}



FreeList::FreeList(size_t dataSize) : _ptr(malloc(dataSize)) {
	_rbTree = new BlackRedTreeForFreeList(_ptr, dataSize);
	_secondTree = new TRedBlackTree<void*, NodeForFreeList*>();
	_dataSize = dataSize;
}

FreeList::~FreeList() {
	delete _ptr;
}

void* FreeList::Allocate(size_t dataSize) {
	NodeForFreeList* node = _rbTree->insert(dataSize);
	if (node != nullptr) {
		void* ptr = std::get<void*>(node->getTuple());
		_secondTree->insert(ptr, node);
		return ptr;
	}
	return nullptr;
}

bool FreeList::Deallocate(void* ptr) {
	void* maxPtr = (char*)_ptr + _dataSize;

	if (ptr > maxPtr || ptr < _ptr) return false;


	NodeForFreeList* node = std::get< NodeForFreeList*>(_secondTree->remove(ptr));

	return _rbTree->remove(node);
}
