#ifndef SMALL_OBJECT_ALLOCATOR_H
#define SMALL_OBJECT_ALLOCATOR_H
#include <vector>
#include "RedBlackTree.h"

class Chunk {

	unsigned char _firstAvailableBlock;
	unsigned char _blocksAvailable;

	unsigned char* _dataPtr;


public:
	Chunk() = default;
	Chunk(const size_t blockSize, const unsigned char blocks);
	void Release();
	void* Allocate(const size_t blockSize);
	void Deallocate(void* p, size_t blockSize);

	unsigned char* Start() const;
	bool isEmpty(const unsigned char blocks) const;
	bool isFull() const;

	bool operator==(const Chunk& other) const {
		return _dataPtr == other._dataPtr;
	}
};


class FixedAllocator {

	size_t _blockSize;

	TRedBlackTree<void*, Chunk> _chunks;
	std::vector<Chunk*> _freeChunks;
	Chunk* _lastAllocatedChunk;
	Chunk* _lastDeallocationChunk = nullptr;

public:
	FixedAllocator(const size_t blockSize, const unsigned char blockPerChunk);
	~FixedAllocator();
	void* Allocate(const unsigned char blockPerChunk);
	bool Deallocate(void* ptr, const unsigned char blockPerChunk);

private:
	void AddChunk(const unsigned char blockPerChunk);
	void DoDeallocate(Chunk* chunk, void* ptr, const unsigned char blockPerChunk, int i = -1);
};

class SmallObjAllocator {

	std::vector<FixedAllocator*> _pool;

	const unsigned char _blockPerChunk = 10llu;


public:
	SmallObjAllocator(size_t smallObjectAllocation);
	void* Allocate(size_t numBytes);
	void Deallocate(void* p);

	int getIndex(size_t size);
};

#endif // !SMALL_OBJECT_ALLOCATOR_H
