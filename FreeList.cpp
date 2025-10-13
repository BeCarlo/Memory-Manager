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
	if (direction == LEFT) _otherData = nodeToDelete->_otherData;
	_dataToSort += nodeToDelete->_dataToSort;
	listNode[direction] = nodeToDelete->listNode[direction];
	if (listNode[direction] != nullptr)
		listNode[direction]->listNode[!direction] = this;
	delete nodeToDelete;
}

void NodeForFreeList::split(NodeForFreeList* node) {
	node->_otherData = (char*)(_otherData)+_dataToSort;
	node->listNode[LEFT] = this;
	node->listNode[RIGHT] = listNode[RIGHT];
	listNode[RIGHT] = node;
}

bool NodeForFreeList::operator>=(const NodeForFreeList* node) const {
	return _dataToSort >= node->_dataToSort;
}

bool NodeForFreeList::operator>=(const size_t& data) const {
	return _dataToSort >= data;
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
		currentNodePosition = (NodePosition)(*currentNode < node);
		currentNode = currentNode->childs[currentNodePosition];
	}

	if (node->parent == nullptr) {
		_root = node;
		_root->color = BLACK;
		return;
	}

	node->parent->childs[currentNodePosition] = node;

	fixInsertViolation(node);
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
				currentNode = currentNode->parent;
				rotation(currentNode, (BRTRotationDirection)parentPosition);
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
	assert(node != nullptr);

	NodeForFreeList* currentNode = node;
	while (currentNode != nullptr && currentNode != _root && currentNode->color == BLACK) {
		NodePosition currentNodePosition = currentNode->parent->getNodePosition(currentNode);
		NodePosition siblingPosition = (NodePosition)(currentNodePosition == LEFT);
		NodeForFreeList* sibling = currentNode->parent->childs[siblingPosition];

		if (sibling->color == RED) {
			sibling->color = BLACK;
			currentNode->parent->color = RED;
			rotation(currentNode->parent, (BRTRotationDirection)currentNodePosition);
			sibling = currentNode->parent->childs[siblingPosition];
		}

		bool isLeftChildBlack = !sibling->childs[LEFT] || sibling->childs[LEFT]->color == BLACK;
		bool isRightChildBlack = !sibling->childs[RIGHT] || sibling->childs[RIGHT]->color == BLACK;

		if (isLeftChildBlack && isRightChildBlack) {
			sibling->color = RED;
			currentNode = currentNode->parent;
		}
		else {
			if (!sibling->childs[siblingPosition] || sibling->childs[siblingPosition]->color == BLACK) {
				if (sibling->childs[currentNodePosition])
					sibling->childs[currentNodePosition]->color = BLACK;
				sibling->color = RED;
				rotation(sibling, (BRTRotationDirection)siblingPosition);
				sibling = currentNode->parent->childs[siblingPosition];
			}
			sibling->color = currentNode->parent->color;
			currentNode->parent->color = BLACK;
			if (sibling->childs[siblingPosition])
				sibling->childs[siblingPosition]->color = BLACK;
			rotation(currentNode->parent, (BRTRotationDirection)currentNodePosition);
			currentNode = _root;
		}
	}
	if (currentNode) currentNode->color = BLACK;
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

	secondNode->childs[rotation] = node;
	node->parent = secondNode;
}

void BlackRedTreeForFreeList::normalRemove(NodeForFreeList* node) {
	NodeForFreeList* replaceNode = nullptr;
	NodeForFreeList* replaceNodeRightChild = nullptr;
	NodeColor originalReplaceNodeColor = node->color;
	NodePosition nodePosition;
	NodePosition currentNodePosition;

	//BST remove
	switch (node->howManyChilds()) {
	case 2:
		replaceNode = node->childs[RIGHT];
		while (replaceNode->childs[LEFT] != nullptr) replaceNode = replaceNode->childs[LEFT];
		originalReplaceNodeColor = replaceNode->color;
		replaceNodeRightChild = replaceNode->childs[RIGHT];


		currentNodePosition = (NodePosition)(replaceNode->parent == node);
		if (replaceNodeRightChild != nullptr) {
			replaceNode->parent->childs[currentNodePosition] = replaceNodeRightChild;
			replaceNodeRightChild->parent = replaceNode->parent;
		}

		node->childs[LEFT]->parent = replaceNode;
		node->childs[RIGHT]->parent = replaceNode;
		replaceNode->childs[LEFT] = node->childs[LEFT];
		replaceNode->childs[RIGHT] = node->childs[RIGHT];
		replaceNode->color = node->color;
		replaceNode->parent = node->parent;
		if (node->parent == nullptr) {
			_root = replaceNode;
			break;
		}
		nodePosition = node->parent->getNodePosition(node);
		node->parent->childs[nodePosition] = replaceNode;
		break;
	case 1:

		replaceNode = node->childs[node->childs[LEFT] == nullptr];
		replaceNodeRightChild = replaceNode;
		replaceNode->parent = node->parent;

	default:
		if (node->parent == nullptr) {
			_root = replaceNode;
			break;
		}
		nodePosition = node->parent->getNodePosition(node);
		node->parent->childs[nodePosition] = replaceNode;
		break;
	}

	if (replaceNodeRightChild != nullptr && originalReplaceNodeColor == BLACK) fixRemoveViolation(replaceNodeRightChild);
	delete node;
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
		currentNodePosition = (NodePosition)(size >= currentNode->getData());
		currentNode = currentNode->childs[currentNodePosition];
	}

	if (currentNode == nullptr) {
		return nullptr;
	}

	split(currentNode, size);

	currentNode->isFree = false;
	return currentNode;

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
		void* ptr = node->getOtherData();
		_secondTree->insert(ptr, node);
		return ptr;
	}
	return nullptr;
}

bool FreeList::Deallocate(void* ptr) {
	void* maxPtr = (char*)_ptr + _dataSize;

	if (ptr > maxPtr || ptr < _ptr) return false;


	NodeForFreeList* node = _secondTree->remove(ptr);

	if (node == nullptr) return false;

	return _rbTree->remove(node);
}
