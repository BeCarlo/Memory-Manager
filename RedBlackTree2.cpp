#include "RedBlackTree2.h"
#include <malloc.h>
#include <cassert>
#include <iostream>
#include <iomanip>


Node::Node(const size_t data) {
	color = RED;
	_dataSize = data;
}

Node::Node(void* ptr) {
	_dataPtr = ptr;
}

Node::Node(const size_t data, void* ptr) {
	color = RED;
	_dataSize = data;
	_dataPtr = ptr;
	childs[0] = nullptr;
	childs[1] = nullptr;
}

inline int Node::howManyChilds() const {
	return (childs[LEFT] != nullptr) + (childs[RIGHT] != nullptr);
}

inline bool Node::hasChilds() const {
	return howManyChilds() != 0;
}

void* Node::getPtr() const {
	return _dataPtr;
}

size_t Node::getSize() const {
	return _dataSize;
}

void Node::setSize(const size_t size) {
	_dataSize = size;
}

bool Node::wouldJoin() {
	return  (leftListNode != nullptr && leftListNode->isFree) || (rightListNode != nullptr && rightListNode->isFree);
}

bool Node::leftListNodeIsFree() {
	return leftListNode != nullptr && leftListNode->isFree;
}

bool Node::rightListNodeIsFree() {
	return rightListNode != nullptr && rightListNode->isFree;
}

void Node::leftJoin() {
	if (!leftListNode->isFree) return;
	_dataSize += leftListNode->_dataSize;
	_dataPtr = leftListNode->_dataPtr;
	Node* toDelete = leftListNode;
	leftListNode = leftListNode->leftListNode;
	if (leftListNode != nullptr)
		leftListNode->rightListNode = this;
	delete toDelete;
}

void Node::rightJoin() {
	if (!rightListNode->isFree) return;
	_dataSize += rightListNode->_dataSize;
	Node* toDelete = rightListNode;
	rightListNode = rightListNode->rightListNode;
	if (rightListNode != nullptr)
		rightListNode->leftListNode = this;
	delete toDelete;
}

void Node::split(Node* rightNone) {
	rightNone->_dataPtr = (char*)_dataPtr + _dataSize;
	rightNone->leftListNode = this;
	rightNone->rightListNode = rightListNode;
	rightListNode = rightNone;
}

Node* Node::getSibling(const Node* node) {
	if (childs[LEFT] != nullptr && node == childs[LEFT])
		return childs[RIGHT];
	return childs[LEFT];
}

NodePosition Node::getNodePosition(const Node* node) {
	if (childs[LEFT] != nullptr && node == childs[LEFT])
		return LEFT;
	return RIGHT;
}

Node* Node::getListNode(NodePosition position) {
	return position ? leftListNode : rightListNode;
}

bool Node::operator==(Node node) {
	return  _dataSize == node._dataSize;
}

bool Node::operator>(Node node) {
	return _dataSize > node._dataSize;
}

bool Node::operator<(Node node) {
	return _dataSize < node._dataSize;
}

bool Node::operator<=(Node node) {
	return _dataSize <= node._dataSize;
}

bool Node::operator>=(Node node) {
	return _dataSize >= node._dataSize;
}

bool Node::operator==(size_t size) {
	return  _dataSize == size;
}
bool Node::operator>(size_t size) {
	return  _dataSize > size;
}
bool Node::operator<(size_t size) {
	return  _dataSize < size;
}
bool Node::operator<=(size_t size) {
	return  _dataSize <= size;
}
bool Node::operator>=(size_t size) {
	return  _dataSize >= size;
}

Node* RedBlackTree::insert(size_t size) {
	Node* currentNode = _root;
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

	return currentNode;
}

void RedBlackTree::rotation(Node* node, NodePosition rotation) {
	NodePosition oppositeRotation = (NodePosition)(rotation == LEFT);
	assert(node != nullptr);
	assert(node->childs[oppositeRotation] != nullptr);

	Node* secondNode = node->childs[oppositeRotation];

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


Node* RedBlackTree::search(void* ptr) {

	if (ptr == nullptr) return nullptr;


	Node* currentNode = _root;
	while (currentNode->getPtr() != ptr) {
		const NodePosition goToRight = (NodePosition)(currentNode->getPtr() >= ptr);
		currentNode = currentNode->getListNode(goToRight);
	}

	return currentNode;
}

bool RedBlackTree::remove(Node* nodeToRemove) {

	if (nodeToRemove == nullptr) return false;

	nodeToRemove->isFree = true;


	return true;

	if (!nodeToRemove->wouldJoin()) {
		return true;
	}

	removeNormal(nodeToRemove);

	if (nodeToRemove->leftListNodeIsFree()) {
		removeNormal(nodeToRemove->leftListNode);
		nodeToRemove->leftJoin();
	}
	if (nodeToRemove->rightListNodeIsFree()) {
		removeNormal(nodeToRemove->rightListNode);
		nodeToRemove->rightJoin();
	}
	nodeToRemove->childs[LEFT] = nullptr;
	nodeToRemove->childs[RIGHT] = nullptr;
	nodeToRemove->color = RED;

	insertFreeNode(nodeToRemove);

	return true;
}

RedBlackTree::RedBlackTree(void* startPtr, size_t size) {

	_root = new Node(size, startPtr);
	_root->color = BLACK;
}

RedBlackTree::~RedBlackTree() {
	deleteAll(_root);
}

void RedBlackTree::fixInsertViolation(Node* node) {
	assert(nullptr != node);

	if (node->parent == nullptr) return;

	Node* currentNode = node;

	while (currentNode != _root && currentNode->parent->color == RED) {
		Node* grandparent = currentNode->parent->parent;
		Node* uncle = grandparent->getSibling(currentNode->parent);
		NodePosition parentPosition = grandparent->getNodePosition(currentNode->parent);
		NodePosition oppositeParentPosition = (NodePosition)(parentPosition == LEFT);

		if (uncle == nullptr || uncle->color == BLACK) {
			if (currentNode->parent->getNodePosition(currentNode) == oppositeParentPosition) {
				rotation(currentNode->parent, parentPosition);
			}
			currentNode->parent->color = BLACK;
			grandparent->color = RED;
			rotation(grandparent, oppositeParentPosition);
			continue;
		}

		currentNode->parent->color = BLACK;
		uncle->color = BLACK;
		grandparent->color = RED;
		currentNode = grandparent;
	}
	_root->color = BLACK;
}


void RedBlackTree::fixRemoveViolation(Node* node) {
	assert(nullptr != node);

	Node* currentNode = node;
	while (currentNode != nullptr && currentNode->parent != nullptr && currentNode->color == BLACK) {
		Node* curretNodeSibling = currentNode->parent->getSibling(currentNode);
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

void RedBlackTree::removeNormal(Node* node) {

	Node* replaceNode = node;
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


void RedBlackTree::insertFreeNode(Node* node) {
	Node* currentNode = _root;
	NodePosition currentNodePosition = LEFT;

	//binary inserction
	while (currentNode != nullptr) {
		node->parent = currentNode;
		currentNodePosition = (NodePosition)(*node >= *currentNode);
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


void RedBlackTree::split(Node* node, size_t size) {

	size_t newSize = node->getSize() - size;
	if (newSize <= MIN_SPLIT) return;

	node->setSize(size);

	Node* newNode = new Node(newSize);
	node->split(newNode);
	insertFreeNode(newNode);
}

void RedBlackTree::deleteAll(Node* node) {
	if (node->childs[LEFT] != nullptr) deleteAll(node->childs[LEFT]);
	if (node->childs[RIGHT] != nullptr) deleteAll(node->childs[RIGHT]);
	delete node;
}

void RedBlackTree::print(Node* node, int indent) {
	if (node != NULL)
	{
		if (node->childs[RIGHT]) print(node->childs[RIGHT], indent + 4);
		if (indent) {
			std::cout << std::setw(indent) << ' ';
		}
		if (node->childs[RIGHT]) std::cout << " /\n" << std::setw(indent) << ' ';

		std::cout << "(" << node->getSize() << ")" << node->getPtr() << "(" << node->isFree << ")" << "\n ";

		if (node->childs[LEFT]) {
			std::cout << std::setw(indent) << ' ' << " \\\n";
			print(node->childs[LEFT], indent + 4);
		}
	}
}

void RedBlackTree::printBT(const Node* node, const std::string& prefix, bool isLeft)
{
	if (node)
	{
		std::cout << prefix;

		std::cout << (isLeft ? "--" : "--");

		// print the value of the node
		std::cout << "(" << node->getSize() << ")" << node << "(" << node->isFree << ")" << std::endl;

		// enter the next tree level - left and right branch
		printBT(node->childs[LEFT], prefix + (isLeft ? "|   " : "    "), true);
		printBT(node->childs[RIGHT], prefix + (isLeft ? "|   " : "    "), false);
	}
}

void RedBlackTree::printFL() {
	Node* current = _root;

	if (current == nullptr) return;

	while (current->leftListNode != nullptr) { current = current->leftListNode; }
	std::cout << current << "->";
	while (current->rightListNode != nullptr) {
		current = current->rightListNode;
		std::cout << current << "->";
	}

	std::cout << std::endl;
}
