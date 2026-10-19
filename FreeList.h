#ifndef FREE_LIST_H
#define FREE_LIST_H

#include "RedBlackTree.h"

#define MIN_SPLIT 255llu

struct NodeForFreeList : public TNode<size_t, void*> {

	NodeForFreeList(const size_t& dataToSort, const void* ptr);


	NodeForFreeList* parent = nullptr;
	NodeForFreeList* childs[2] = { nullptr, nullptr };

	NodeForFreeList* getSibling(const NodeForFreeList* node) const;

	NodePosition getNodePosition(const NodeForFreeList* node) const {
		return (NodePosition)(childs[RIGHT] != nullptr && node == childs[RIGHT]);
	}

	typedef NodePosition Direction;

	NodeForFreeList* listNode[2] = { nullptr, nullptr };
	bool isFree = true;

	bool wouldJoin() const;
	bool listNodeIsFree(NodePosition position) const;

	void join(Direction direction);

	void split(NodeForFreeList* rightNode);

	bool operator >= (const NodeForFreeList* node) const;

	bool operator >= (const size_t& data) const;


};

struct BlackRedTreeForFreeList;

template<> class TRedBlackTree<NodeForFreeList, void*> {
	NodeForFreeList* _root = nullptr;

	friend struct BlackRedTreeForFreeList;
};

struct BlackRedTreeForFreeList : public TRedBlackTree<NodeForFreeList, void*> {

	void split(NodeForFreeList* node, size_t size);

	void insertFreeNode(NodeForFreeList* node);

	void fixInsertViolation(NodeForFreeList* node);
	void fixRemoveViolation(NodeForFreeList* node);

	void rotation(NodeForFreeList* node, const BRTRotationDirection rotation);

	void normalRemove(NodeForFreeList* node);

public:
	BlackRedTreeForFreeList(const void* startPtr, const size_t& size);

	NodeForFreeList* insert(const size_t& size);
	bool remove(NodeForFreeList* nodeToRemove);
};



class FreeList {
	void* _ptr;
	BlackRedTreeForFreeList* _rbTree;
	TRedBlackTree<void*, NodeForFreeList*>* _secondTree;
	size_t _dataSize;

public:
	FreeList(size_t dataSize);
	~FreeList();

	void* Allocate(size_t dataSize);
	bool Deallocate(void* ptr);
};

#endif // !FREE_LIST_H