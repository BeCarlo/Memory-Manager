#include <string>
#ifndef BLACKREDTREE_H
#define BLACKREDTREE_H

#define MIN_SPLIT 255llu

enum RedBlackTreeColor {
	BLACK,
	RED
};

enum NodePosition {
	LEFT,
	RIGHT
};

class Node {
protected:
	size_t _dataSize = 0;
	void* _dataPtr = nullptr;

public:

	Node* leftListNode = nullptr;
	Node* rightListNode = nullptr;

	RedBlackTreeColor color = RED;

	Node* parent = nullptr;
	Node* childs[2] = { nullptr, nullptr };
	
	bool isFree = true;

	Node(const size_t data);
	Node(void* ptr);
	Node(const size_t data, void* ptr);

	inline int howManyChilds() const;
	inline bool hasChilds() const;

	void* getPtr() const;

	size_t getSize() const;
	void setSize(const size_t size);

	bool wouldJoin();
	bool leftListNodeIsFree();
	bool rightListNodeIsFree();

	void leftJoin();
	void rightJoin();

	void split(Node* rightNone);

	Node* getSibling(const Node* node);
	NodePosition getNodePosition(const Node* node);

	Node* getListNode(NodePosition position);

	bool operator ==(Node node);
	bool operator >(Node node);
	bool operator <(Node node);
	bool operator <=(Node node);
	bool operator >=(Node node);

	bool operator ==(size_t size);
	bool operator >(size_t size);
	bool operator <(size_t size);
	bool operator <=(size_t size);
	bool operator >=(size_t size);
};

class RedBlackTree {
private:
	Node* _root = nullptr;

public:
	Node* insert(const size_t size);
	void rotation(Node* node, NodePosition rotation);
	Node* search(void* ptr);
	bool remove(Node* ptr);
	RedBlackTree() = default;
	RedBlackTree(void* startPtr, size_t size);
	~RedBlackTree();

private:
	void fixInsertViolation(Node* node);
	void fixRemoveViolation(Node* node);

	void removeNormal(Node* node);


	void insertFreeNode(Node* node);
	void split(Node* node, size_t size);

	void deleteAll(Node* node);
	void print(Node* node, int indent = 0);
	void printBT(const Node* node, const std::string& prefix = "", bool isLeft=false);
	void printFL();
};

#endif // !BLACKREDTREE_H
