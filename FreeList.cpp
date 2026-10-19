#include "FreeList.h"
#include <corecrt_malloc.h>
#include <cassert>
#include <cstddef>


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
	if (listNode[RIGHT] != nullptr)
		listNode[RIGHT]->listNode[LEFT] = node;
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

void BlackRedTreeForFreeList::normalRemove(NodeForFreeList* z) {
	auto transplant = [this](NodeForFreeList* u, NodeForFreeList* v) {
		if (u->parent == nullptr) _root = v;
		else u->parent->childs[u->parent->getNodePosition(u)] = v;
		if (v != nullptr) v->parent = u->parent;
	};

	NodeForFreeList* y = z;
	NodeColor yOriginalColor = y->color;
	NodeForFreeList* x = nullptr;
	NodeForFreeList* xParent = nullptr;

	if (z->childs[LEFT] == nullptr) {
		x = z->childs[RIGHT];
		xParent = z->parent;
		transplant(z, z->childs[RIGHT]);
	}
	else if (z->childs[RIGHT] == nullptr) {
		x = z->childs[LEFT];
		xParent = z->parent;
		transplant(z, z->childs[LEFT]);
	}
	else {
		y = z->childs[RIGHT];
		while (y->childs[LEFT] != nullptr) y = y->childs[LEFT];
		yOriginalColor = y->color;
		x = y->childs[RIGHT];
		if (y->parent == z) {
			xParent = y;
		}
		else {
			xParent = y->parent;
			transplant(y, y->childs[RIGHT]);
			y->childs[RIGHT] = z->childs[RIGHT];
			y->childs[RIGHT]->parent = y;
		}
		transplant(z, y);
		y->childs[LEFT] = z->childs[LEFT];
		y->childs[LEFT]->parent = y;
		y->color = z->color;
	}

	if (yOriginalColor != BLACK) return;

	NodeForFreeList* node = x;
	NodeForFreeList* parent = xParent;
	while (node != _root && (node == nullptr || node->color == BLACK)) {
		NodePosition dir = (parent->childs[LEFT] == node) ? LEFT : RIGHT;
		NodePosition other = (NodePosition)(dir == LEFT ? RIGHT : LEFT);
		NodeForFreeList* w = parent->childs[other];

		if (w->color == RED) {
			w->color = BLACK;
			parent->color = RED;
			rotation(parent, (BRTRotationDirection)dir);
			w = parent->childs[other];
		}

		bool wDirBlack = (w->childs[dir] == nullptr || w->childs[dir]->color == BLACK);
		bool wOtherBlack = (w->childs[other] == nullptr || w->childs[other]->color == BLACK);

		if (wDirBlack && wOtherBlack) {
			w->color = RED;
			node = parent;
			parent = node->parent;
		}
		else {
			if (wOtherBlack) {
				if (w->childs[dir] != nullptr) w->childs[dir]->color = BLACK;
				w->color = RED;
				rotation(w, (BRTRotationDirection)other);
				w = parent->childs[other];
			}
			w->color = parent->color;
			parent->color = BLACK;
			if (w->childs[other] != nullptr) w->childs[other]->color = BLACK;
			rotation(parent, (BRTRotationDirection)dir);
			node = _root;
			parent = nullptr;
		}
	}
	if (node != nullptr) node->color = BLACK;
}

BlackRedTreeForFreeList::BlackRedTreeForFreeList(const void* startPtr, const size_t& size) {
	_root = new NodeForFreeList(size, startPtr);
	_root->color = BLACK;
}

NodeForFreeList* BlackRedTreeForFreeList::insert(const size_t& size) {
	NodeForFreeList* currentNode = _root;
	while (currentNode != nullptr && !(*currentNode >= size)) {
		currentNode = currentNode->childs[(NodePosition)(size >= currentNode->getData())];
	}

	if (currentNode == nullptr) {
		return nullptr;
	}

	normalRemove(currentNode);
	currentNode->parent = nullptr;
	currentNode->childs[LEFT] = nullptr;
	currentNode->childs[RIGHT] = nullptr;

	split(currentNode, size);

	currentNode->isFree = false;
	return currentNode;
}

bool BlackRedTreeForFreeList::remove(NodeForFreeList* nodeToRemove) {
	if (nodeToRemove == nullptr) return false;

	nodeToRemove->isFree = true;

	if (nodeToRemove->listNodeIsFree(RIGHT)) {
		normalRemove(nodeToRemove->listNode[RIGHT]);
		nodeToRemove->join(RIGHT);
	}
	if (nodeToRemove->listNodeIsFree(LEFT)) {
		normalRemove(nodeToRemove->listNode[LEFT]);
		nodeToRemove->join(LEFT);
	}

	nodeToRemove->parent = nullptr;
	nodeToRemove->childs[LEFT] = nullptr;
	nodeToRemove->childs[RIGHT] = nullptr;
	nodeToRemove->color = RED;
	nodeToRemove->isFree = true;
	insertFreeNode(nodeToRemove);

	return true;
}



FreeList::FreeList(size_t dataSize) : _ptr(malloc(dataSize)) {
	_dataSize = (_ptr != nullptr) ? dataSize : 0;
	_rbTree = new BlackRedTreeForFreeList(_ptr, _dataSize);
	_secondTree = new TRedBlackTree<void*, NodeForFreeList*>();
}

FreeList::~FreeList() {
	free(_ptr);
	delete _rbTree;
	delete _secondTree;
}

void* FreeList::Allocate(size_t dataSize) {
	const size_t ALIGN = alignof(std::max_align_t);
	dataSize = (dataSize + (ALIGN - 1)) & ~(ALIGN - 1);

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
