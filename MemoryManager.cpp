#include "MemoryManager.h"
#include <exception>
#include <iostream>

static SmallObjAllocator sObjAlloc = SmallObjAllocator(SMALL_OBJECT_SIZE);
static FreeList freeList = FreeList(1024 * 1024 * 1024);


void* my_allocate(std::size_t size) {
	if (size == 0llu) ++size;

	if (size > SMALL_OBJECT_SIZE) {

		if (void* ptr = freeList.Allocate(size)) return ptr;

		throw std::exception("bad allocation", 1);
	}

	return sObjAlloc.Allocate(size);
}

void my_deallocate(void* ptr) {

	if (freeList.Deallocate(ptr)) return;

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
