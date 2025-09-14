#ifndef SECOND_TREE_H
#define SECOND_TREE_H

#include "RedBlackTree2.h"

class SecondNode {
protected:
	void* _dataPtr = nullptr;
	Node* _node = nullptr;

public:

	RedBlackTreeColor color = RED;

	SecondNode* parent = nullptr;
	SecondNode* childs[2] = { nullptr, nullptr };

	bool isFree = true;

	SecondNode(void* dataPtr, Node* node);

	inline int howManyChilds() const;
	inline bool hasChilds() const;

	SecondNode* getSibling(const SecondNode* node);
	NodePosition getNodePosition(const SecondNode* node);

	Node* getNode() { return _node; }
	void* getPtr() const { return _dataPtr; }

	SecondNode* getListNode(NodePosition position);

	bool operator ==(SecondNode node);
	bool operator >(SecondNode node);
	bool operator <(SecondNode node);
	bool operator <=(SecondNode node);
	bool operator >=(SecondNode node);

	bool operator ==(void* size);
	bool operator >(void* size);
	bool operator <(void* size);
	bool operator <=(void* size);
	bool operator >=(void* size);
};

class SecondRedBlackTree {
private:
	SecondNode* _root = nullptr;

public:
	void insert(void* ptr, Node* nodeToAdd);
	void rotation(SecondNode* node, NodePosition rotation);
	SecondNode* search(void* ptr);
	Node* remove(void* ptr);
	SecondRedBlackTree() = default;
	~SecondRedBlackTree();

private:
	void fixInsertViolation(SecondNode* node);
	void fixRemoveViolation(SecondNode* node);

	void deleteAll(SecondNode* node);
};

#endif // !SECOND_TREE_H