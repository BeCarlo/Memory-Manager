#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#define SMALL_OBJECT_SIZE 255llu

#include "SmallObjectAllocator.h"
#include "FreeList.h"


void* my_allocate(size_t _size);

void my_deallocate(void* ptr);


struct CustomMenoryManager {
	void* operator new(size_t sz);
	void* operator new[](size_t sz);
	void operator delete(void* ptr);
	void operator delete[](void* ptr);
};

#define CUSTOMMEMORY : public CustomMenoryManager

#endif // !MEMORY_MANAGER_H
