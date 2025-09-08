#include "FreeList.h"
#include <corecrt_malloc.h>

FreeList::FreeList(size_t dataSize) : _ptr(malloc(dataSize)) {
	_rbTree = RedBlackTree(_ptr, dataSize);
	_dataSize = dataSize;
}

FreeList::~FreeList() {
	delete _ptr;
}

void* FreeList::Allocate(size_t dataSize) {
	return _rbTree.insert(dataSize);
}

bool FreeList::Deallocate(void* ptr) {
	void* maxPtr = (char*)_ptr + _dataSize;

	if (ptr > maxPtr || ptr < maxPtr) return false;

	return _rbTree.remove(ptr);
}
