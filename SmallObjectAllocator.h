#ifndef SMALL_OBJECT_ALLOCATOR_H
#define SMALL_OBJECT_ALLOCATOR_H
#include <vector>

#define CHUNK_SIZE ((size_t)4096)
#define CHUNK_HEADER_SIZE ((size_t)16)
#define CHUNK_SLAB_SIZE ((size_t)(256 * 1024))
#define MAX_BLOCKS_PER_CHUNK ((size_t)255)

class Chunk {

	unsigned char _firstAvailableBlock;
	unsigned char _blocksAvailable;
	unsigned char _blocks;
	unsigned char _sizeClass;

public:
	static Chunk* Init(void* memory, const size_t blockSize, const unsigned char sizeClass);
	static Chunk* Owner(void* ptr);

	void* Allocate(const size_t blockSize);
	void Deallocate(void* p, const size_t blockSize);

	unsigned char* Start() const;
	unsigned char sizeClass() const;
	bool isEmpty() const;
	bool isFull() const;
};


class ChunkStore {

	std::vector<void*> _slabs;
	std::vector<Chunk*> _spare;

public:
	~ChunkStore();
	Chunk* acquire();
	void release(Chunk* chunk);
};


class FixedAllocator {

	size_t _blockSize = 0;
	unsigned char _sizeClass = 0;
	ChunkStore* _store = nullptr;

	std::vector<Chunk*> _freeChunks;
	Chunk* _lastAllocatedChunk = nullptr;
	Chunk* _lastDeallocationChunk = nullptr;

public:
	FixedAllocator(const size_t blockSize, const unsigned char sizeClass, ChunkStore* store);
	void* Allocate();
	void Deallocate(void* ptr, Chunk* chunk);

private:
	void dropFromFreeChunks(Chunk* chunk);
};

class SmallObjAllocator {

	ChunkStore _store;
	std::vector<FixedAllocator*> _pool;

public:
	SmallObjAllocator(size_t smallObjectAllocation);
	~SmallObjAllocator();
	void* Allocate(size_t numBytes);
	void Deallocate(void* p) const;

	static unsigned int getIndex(size_t size);
};

#endif // !SMALL_OBJECT_ALLOCATOR_H
