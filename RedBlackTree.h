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

		if (currentNode->getData() == data) return currentNode;

		NodePosition currentNodePosition = (NodePosition)(data >= currentNode->getData());
		currentNode = currentNode->childs[currentNodePosition];
	}

	return currentNode;
}

template<typename TypeToSort, typename OtherType>
__forceinline OtherType TRedBlackTree<TypeToSort, OtherType>::remove(const TypeToSort data) {
	typedef TNode<TypeToSort, OtherType> Node;

	Node* z = search(data);
	if (z == nullptr) return OtherType();
	const OtherType returnData = z->getOtherData();

	auto transplant = [this](Node* u, Node* v) {
		if (u->parent == nullptr) _root = v;
		else u->parent->childs[u->parent->getNodePosition(u)] = v;
		if (v != nullptr) v->parent = u->parent;
	};

	Node* y = z;
	NodeColor yOriginalColor = y->color;
	Node* x = nullptr;
	Node* xParent = nullptr;

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

	delete z;

	if (yOriginalColor == BLACK) {
		Node* node = x;
		Node* parent = xParent;
		while (node != _root && (node == nullptr || node->color == BLACK)) {
			NodePosition dir = (parent->childs[LEFT] == node) ? LEFT : RIGHT;
			NodePosition other = (NodePosition)(dir == LEFT ? RIGHT : LEFT);
			Node* w = parent->childs[other];

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

template<typename TypeToSort, typename OtherType>
__forceinline void TRedBlackTree<TypeToSort, OtherType>::deleteAll(TNode<TypeToSort, OtherType>* node) {
	if (node == nullptr) return;
	if (node->childs[LEFT] != nullptr) deleteAll(node->childs[LEFT]);
	if (node->childs[RIGHT] != nullptr) deleteAll(node->childs[RIGHT]);
	delete node;
}



#endif // !BLACK_RED_TREE_H