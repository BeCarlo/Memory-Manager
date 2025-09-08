#ifndef FREE_LIST_H
#define FREE_LIST_H

#include "RedBlackTree.h"

class FreeList {
	RedBlackTree _rbTree;
	void* _ptr;
	size_t _dataSize;

public:
	FreeList(size_t dataSize);
	~FreeList();
	
	void* Allocate(size_t dataSize);
	bool Deallocate(void* ptr);
};

#endif // !FREE_LIST_H