#include "MemoryManager.h"
#include <exception>
#include <iostream>
#include "RedBlackTree.h"

static SmallObjAllocator sObjAlloc = SmallObjAllocator(MAX_SMALL_OBJECT_SIZE);
static RedBlackTree rbTree = RedBlackTree();

void* my_allocate(std::size_t size) {
	if (size == 0llu) ++size;

	if (size > MAX_SMALL_OBJECT_SIZE) {
		if (void* ptr = rbTree.insert(size)) return ptr;

		throw std::exception("bad allocation", 1);
	}

	return sObjAlloc.Allocate(size);
}

void my_deallocate(void* ptr) {
	if (rbTree.remove(ptr)) return;

	sObjAlloc.Deallocate(ptr);
}


void* CustomMenoryManager::operator new(size_t sz) {
	if (void* ptr = my_allocate(sz))
		return ptr;

	throw std::bad_alloc{};
}

void* CustomMenoryManager::operator new[](size_t sz) {
	return operator new(sz);
}

void CustomMenoryManager::operator delete(void* ptr) {
	my_deallocate(ptr);
}

void CustomMenoryManager::operator delete[](void* ptr) {
	operator delete(ptr);
}
