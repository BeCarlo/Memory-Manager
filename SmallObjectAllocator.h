#ifndef SMALL_OBJECT_ALLOCATOR_H
#define SMALL_OBJECT_ALLOCATOR_H
#include <vector>

class Chunk {

	unsigned char _firstAvailableBlock;
	unsigned char _blocksAvailable;

	unsigned char* _dataPtr;

public:
	Chunk(const size_t blockSize, const unsigned char blocks);
	void Release();
	void* Allocate(const size_t blockSize);
	void Deallocate(void* p, size_t blockSize);
	
	unsigned char* Start() const;
	bool isEmpty(const unsigned char blocks) const;
};


class FixedAllocator {

	size_t _blockSize;

	std::vector<Chunk> _chunks;
	Chunk* _lastAllocatedChunk;
	Chunk* _lastDeallocationChunk = nullptr;

public:
	FixedAllocator(const size_t blockSize, const size_t maxChunkSize, const unsigned char maxBlockPerChunk);
	~FixedAllocator();
	void* Allocate(const size_t maxChunkSize, const unsigned char maxBlockPerChunk);
	bool Deallocate(void* ptr, const size_t maxChunkSize, const unsigned char maxBlockPerChunk);

	unsigned char getNumberOfBlocks(const size_t maxChunkSize, const unsigned char maxBlockPerChunk) const;
};

class SmallObjAllocator {

	std::vector<FixedAllocator*> _pool;
	//this 4 means the quadruple size of maxSmallObjectAllocation
	//on class construction the constructor calculate it
	size_t _maxChunkSize = 4llu;
	unsigned char _maxBlockPerChunk = 100llu;


public:
	SmallObjAllocator(size_t maxSmallObjectAllocation);
	void* Allocate(size_t numBytes);
	void Deallocate(void* p);

	int getIndex(size_t size);
};

#endif // !SMALL_OBJECT_ALLOCATOR_H
