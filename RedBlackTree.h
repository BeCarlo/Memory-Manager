#ifndef BLACKREDTREE_H
#define BLACKREDTREE_H

enum RedBlackTreeColor {
	BLACK,
	RED
};

enum NodePosition {
	LEFT,
	RIGHT
};

struct Node {
	RedBlackTreeColor color;

	Node* parent = nullptr;
	Node* childs[2] = { nullptr, nullptr };

	void* data;
	size_t dataSize;

	Node(const size_t size);
	~Node();

	int howManyChilds() const;
	bool hasChilds() const;

	Node* getSibling(const Node* node);
	NodePosition getNodePosition(const Node* node);
};

class RedBlackTree {
private:
	Node* root = nullptr;


public:
	void* insert(const size_t size);
	void rotation(Node* node, NodePosition rotation);
	Node* search(void* ptr);
	bool remove(void* ptr);

	~RedBlackTree();

private:
	void fixInsertViolation(Node* node);
	void fixRemoveViolation(Node* node);

	void deleteAll(Node* node);
};


#endif // !BLACKREDTREE_H
