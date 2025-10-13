#ifndef BLACK_RED_TREE_H
#define BLACK_RED_TREE_H

#include "BRTreeNode.h"

typedef NodePosition BRTRotationDirection;

template<class TypeToSort, typename OtherType>
class TRedBlackTree {
	TNode<TypeToSort, OtherType>* _root = nullptr;

public:
	TRedBlackTree() = default;
	TRedBlackTree(const TypeToSort& dataToSort, const OtherType& otherData);
	~TRedBlackTree();

	__forceinline TNode<TypeToSort, OtherType>* insert(const TypeToSort& dataToSort, const OtherType& otherData);
	__forceinline TNode<TypeToSort, OtherType>* search(const TypeToSort& data);
	__forceinline OtherType remove(const TypeToSort data);

	__forceinline TNode<TypeToSort, OtherType>* getRoot() const { return _root; }

private:
	__forceinline void rotation(TNode<TypeToSort, OtherType>* node, const BRTRotationDirection rotation);

	__forceinline void fixInsertViolation(TNode<TypeToSort, OtherType>* node);
	__forceinline void fixRemoveViolation(TNode<TypeToSort, OtherType>* node);

	__forceinline void deleteAll(TNode<TypeToSort, OtherType>* node);
};

//IMPLEMENTATION


template<typename TypeToSort, typename OtherType>
TRedBlackTree<TypeToSort, OtherType>::TRedBlackTree(const TypeToSort& dataToSort, const OtherType& otherData) {
	_root = new TNode<TypeToSort, OtherType>(dataToSort, otherData);
	_root->color = BLACK;
}

template<typename TypeToSort, typename OtherType>
TRedBlackTree<TypeToSort, OtherType>::~TRedBlackTree() {
	deleteAll(_root);
}

template<typename TypeToSort, typename OtherType>
__forceinline TNode<TypeToSort, OtherType>* TRedBlackTree<TypeToSort, OtherType>::insert(const TypeToSort& dataToSort, const OtherType& otherData) {

	TNode<TypeToSort, OtherType>* newNode = new TNode<TypeToSort, OtherType>(dataToSort, otherData);
	TNode<TypeToSort, OtherType>* currentNode = _root;
	NodePosition currentNodePosition = LEFT;

	//binary inserction

	while (currentNode != nullptr) {
		newNode->parent = currentNode;
		currentNodePosition = (NodePosition)(*newNode >= currentNode);
		currentNode = currentNode->childs[currentNodePosition];
	}

	if (newNode->parent == nullptr) {
		_root = newNode;
		_root->color = BLACK;
		return _root;
	}

	newNode->parent->childs[currentNodePosition] = newNode;

	fixInsertViolation(newNode);

	return newNode;
}

template<typename TypeToSort, typename OtherType>
__forceinline TNode<TypeToSort, OtherType>* TRedBlackTree<TypeToSort, OtherType>::search(const TypeToSort& data) {

	if (data == nullptr) return nullptr;

	TNode<TypeToSort, OtherType>* currentNode = _root;

	while (currentNode != nullptr) {

		if (currentNode == data) return currentNode;
		if (!currentNode->hasChilds())
			return nullptr;

		NodePosition currentNodePosition = (NodePosition)(data >= currentNode);
		currentNode = currentNode->childs[currentNodePosition];
	}

	return currentNode;
}

template<typename TypeToSort, typename OtherType>
__forceinline OtherType TRedBlackTree<TypeToSort, OtherType>::remove(const TypeToSort data) {
	TNode<TypeToSort, OtherType>* node = search(data);
	if (node == nullptr) return OtherType();
	TNode<TypeToSort, OtherType>* replaceNode = nullptr;
	TNode<TypeToSort, OtherType>* replaceNodeRightChild = nullptr;
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
	const OtherType returnData = node->getOtherData();
	node->childs[LEFT] = nullptr;
	node->childs[RIGHT] = nullptr;
	node->parent = nullptr;
	delete node;
	return returnData;
}


template<typename TypeToSort, typename OtherType>
__forceinline void TRedBlackTree<TypeToSort, OtherType>::rotation(TNode<TypeToSort, OtherType>* node, const BRTRotationDirection rotation) {
	const BRTRotationDirection oppositeRotation = (NodePosition)(rotation == LEFT);

	assert(node != nullptr);
	assert(node->childs[oppositeRotation] != nullptr);

	TNode<TypeToSort, OtherType>* secondNode = node->childs[oppositeRotation];

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

template<typename TypeToSort, typename OtherType>
__forceinline void TRedBlackTree<TypeToSort, OtherType>::fixInsertViolation(TNode<TypeToSort, OtherType>* node) {
	assert(nullptr != node);

	if (node->parent == nullptr) return;

	TNode<TypeToSort, OtherType>* currentNode = node;

	while (currentNode != _root && currentNode->parent->color == RED) {

		TNode<TypeToSort, OtherType>* grandparent = currentNode->parent->parent;
		TNode<TypeToSort, OtherType>* uncle = grandparent->getSibling(currentNode->parent);
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

template<typename TypeToSort, typename OtherType>
__forceinline void TRedBlackTree<TypeToSort, OtherType>::fixRemoveViolation(TNode<TypeToSort, OtherType>* node) {
	assert(node != nullptr);

	TNode<TypeToSort, OtherType>* currentNode = node;
	while (currentNode != nullptr && currentNode != _root && currentNode->color == BLACK) {
		NodePosition currentNodePosition = currentNode->parent->getNodePosition(currentNode);
		NodePosition siblingPosition = (NodePosition)(currentNodePosition == LEFT);
		TNode<TypeToSort, OtherType>* sibling = currentNode->parent->childs[siblingPosition];

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

/*template<typename TypeToSort, typename OtherType>
__forceinline void TRedBlackTree<TypeToSort, OtherType>::fixRemoveViolation(TNode<TypeToSort, OtherType>* node) {
	assert(nullptr != node);

	TNode<TypeToSort, OtherType>* currentNode = node;
	while (currentNode != nullptr && currentNode->parent != nullptr && currentNode->color == BLACK) {
		NodePosition currentNodePosition = currentNode->parent->getNodePosition(currentNode);
		NodePosition oppositePosition = (NodePosition)(currentNodePosition == LEFT);
		TNode<TypeToSort, OtherType>* curretNodeSibling = currentNode->parent->childs[oppositePosition];

		if (curretNodeSibling->color == RED) {
			curretNodeSibling->color = BLACK;
			currentNode->parent->color = RED;
			rotation(currentNode->parent, (BRTRotationDirection)currentNodePosition);
			curretNodeSibling = currentNode->parent->childs[oppositePosition];
		}

		bool isLeftChildBlack = curretNodeSibling->childs[LEFT] == nullptr || curretNodeSibling->childs[LEFT]->color == BLACK;
		bool isRightChildBlack = curretNodeSibling->childs[RIGHT] == nullptr || curretNodeSibling->childs[RIGHT]->color == BLACK;
		if (isLeftChildBlack && isRightChildBlack) {
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
		}

		curretNodeSibling->color = currentNode->parent->color;
		currentNode->parent->color = BLACK;
		if (curretNodeSibling->childs[oppositePosition] != nullptr)
			curretNodeSibling->childs[oppositePosition]->color = BLACK;
		rotation(currentNode->parent, (BRTRotationDirection)currentNodePosition);
		currentNode = _root;
	}
	if (currentNode != nullptr) currentNode->color = BLACK;
}*/

template<typename TypeToSort, typename OtherType>
__forceinline void TRedBlackTree<TypeToSort, OtherType>::deleteAll(TNode<TypeToSort, OtherType>* node) {
	if (node->childs[LEFT] != nullptr) deleteAll(node->childs[LEFT]);
	if (node->childs[RIGHT] != nullptr) deleteAll(node->childs[RIGHT]);
	delete node;
}



#endif // !BLACK_RED_TREE_H