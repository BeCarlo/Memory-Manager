#include "RedBlackTree.h"

#include <assert.h> 

template<class TypeToSort, class ...Types>
TRedBlackTree<TypeToSort, Types...>::TRedBlackTree(const TypeToSort& dataToSort, const Types&... otherData) {
	_root = new TNode<TypeToSort, Types...>(dataToSort, otherData...);
	_root->color = BLACK;
}

template<class TypeToSort, class ...Types>
TRedBlackTree<TypeToSort, Types...>::~TRedBlackTree() {
	deleteAll(_root);
}

template<class TypeToSort, class ...Types>
__forceinline TNode<TypeToSort, Types...>* TRedBlackTree<TypeToSort, Types...>::insert(const TypeToSort& dataToSort, const Types & ...otherData) {

	TNode<TypeToSort, Types...>* newNode = new TNode<TypeToSort, Types...>(dataToSort, otherData...);
	TNode<TypeToSort, Types...>* currentNode = _root;
	NodePosition currentNodePosition = LEFT;

	//binary inserction

	while (currentNode != nullptr) {
		newNode->parent = currentNode;
		currentNodePosition = (NodePosition)(*currentNode >= *newNode);
		currentNode = currentNode->childs[currentNodePosition];
	}

	if (newNode->parent == nullptr) {
		_root = newNode;
		_root->color = BLACK;
		return;
	}

	newNode->parent->childs[currentNodePosition] = newNode;

	if (newNode->parent->color == RED) fixInsertViolation(newNode);
}

template<class TypeToSort, class ...Types>
__forceinline TNode<TypeToSort, Types...>* TRedBlackTree<TypeToSort, Types...>::search(const TypeToSort& data) {

	if (data == nullptr) return nullptr;

	TNode<TypeToSort, Types...>* currentNode = _root;

	while (currentNode != nullptr) {

		if (*currentNode == data) return currentNode;
		if (!currentNode->hasChilds()) return nullptr;

		NodePosition currentNodePosition = (NodePosition)(*currentNode >= data);
		currentNode = currentNode->childs[currentNodePosition];
	}

	return currentNode;
}

template<class TypeToSort, class ...Types>
__forceinline std::tuple<Types...> TRedBlackTree<TypeToSort, Types...>::remove(const TypeToSort data) {
	TNode<TypeToSort, Types...>* node = search(data);
	TNode<TypeToSort, Types...>* replaceNode = node;
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
	std::tuple<Types...> returnTuple = node->getTuple();
	delete node;
	return returnTuple;
}

template<class TypeToSort, class ...Types>
__forceinline void TRedBlackTree<TypeToSort, Types...>::rotation(TNode<TypeToSort, Types...>* node, const BRTRotationDirection rotation) {
	const BRTRotationDirection oppositeRotation = !rotation;

	assert(node != nullptr);
	assert(node->childs[oppositeRotation] != nullptr);

	TNode<TypeToSort, Types...>* secondNode = node->childs[oppositeRotation];

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

template<class TypeToSort, class ...Types>
__forceinline void TRedBlackTree<TypeToSort, Types...>::fixInsertViolation(const TNode<TypeToSort, Types...>* node) {
	assert(nullptr != node);

	if (node->parent == nullptr) return;

	TNode<TypeToSort, Types...>* currentNode = node;

	while (currentNode != _root && currentNode->parent->color == RED) {
		TNode<TypeToSort, Types...>* grandparent = currentNode->parent->parent;
		TNode<TypeToSort, Types...>* uncle = grandparent->getSibling(currentNode->parent);
		NodePosition parentPosition = grandparent->getNodePosition(currentNode->parent);
		NodePosition oppositeParentPosition = !parentPosition;

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

template<class TypeToSort, class ...Types>
__forceinline void TRedBlackTree<TypeToSort, Types...>::fixRemoveViolation(const TNode<TypeToSort, Types...>* node) {
	assert(nullptr != node);

	TNode<TypeToSort, Types...>* currentNode = node;
	while (currentNode != nullptr && currentNode->parent != nullptr && currentNode->color == BLACK) {
		TNode<TypeToSort, Types...>* curretNodeSibling = currentNode->parent->getSibling(currentNode);
		NodePosition currentNodePosition = currentNode->parent->getNodePosition(currentNode);
		NodePosition oppositePosition = !currentNodePosition;

		if (curretNodeSibling == nullptr) {
			currentNode = currentNode->parent;
			continue;
		}

		if (curretNodeSibling->color == RED) {
			curretNodeSibling->color = BLACK;
			currentNode->parent->color = RED;
			rotation(currentNode->parent, (BRTRotationDirection)currentNodePosition);
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
			rotation(curretNodeSibling, (BRTRotationDirection)oppositePosition);
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
		rotation(currentNode->parent, (BRTRotationDirection)currentNodePosition);
		currentNode = _root;
	}
	if (currentNode != nullptr) currentNode->color = BLACK;
}

template<class TypeToSort, class ...Types>
__forceinline void TRedBlackTree<TypeToSort, Types...>::deleteAll(TNode<TypeToSort, Types...>* node) {
	if (node->childs[LEFT] != nullptr) deleteAll(node->childs[LEFT]);
	if (node->childs[RIGHT] != nullptr) deleteAll(node->childs[RIGHT]);
	delete node;
}
