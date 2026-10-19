#include "SmallObjectAllocator.h"
#include <intrin.h>
#include <malloc.h>
#include <cstdint>
#include <algorithm>
#include <cassert>

Chunk* Chunk::Init(void* memory, const size_t blockSize, const unsigned char sizeClass) {

	size_t blocks = (CHUNK_SIZE - CHUNK_HEADER_SIZE) / blockSize;
	if (blocks > MAX_BLOCKS_PER_CHUNK) blocks = MAX_BLOCKS_PER_CHUNK;

	Chunk* chunk = static_cast<Chunk*>(memory);
	chunk->_firstAvailableBlock = 0;
	chunk->_blocksAvailable = static_cast<unsigned char>(blocks);
	chunk->_blocks = static_cast<unsigned char>(blocks);
	chunk->_sizeClass = sizeClass;

	unsigned char* ptr = chunk->Start();
	for (unsigned char i = 0; i < blocks; ptr += blockSize) {
		*ptr = ++i;
	}

	return chunk;
}

Chunk* Chunk::Owner(void* ptr) {
	return reinterpret_cast<Chunk*>(reinterpret_cast<uintptr_t>(ptr) & ~(static_cast<uintptr_t>(CHUNK_SIZE) - 1));
}

void* Chunk::Allocate(const size_t blockSize) {

	if (_blocksAvailable == 0) return nullptr;

	--_blocksAvailable;

	unsigned char* currentBlock = Start() + _firstAvailableBlock * blockSize;

	_firstAvailableBlock = *currentBlock;

	return currentBlock;
}

void Chunk::Deallocate(void* ptr, const size_t blockSize) {

	unsigned char* ptrInChar = static_cast<unsigned char*>(ptr);

	*ptrInChar = _firstAvailableBlock;

	_firstAvailableBlock = static_cast<unsigned char>((ptrInChar - Start()) / blockSize);

	++_blocksAvailable;
}

unsigned char* Chunk::Start() const {
	return reinterpret_cast<unsigned char*>(const_cast<Chunk*>(this)) + CHUNK_HEADER_SIZE;
}

unsigned char Chunk::sizeClass() const {
	return _sizeClass;
}

bool Chunk::isEmpty() const {
	return _blocksAvailable == _blocks;
}

bool Chunk::isFull() const {
	return _blocksAvailable == 0;
}


ChunkStore::~ChunkStore() {
	for (void* slab : _slabs) _aligned_free(slab);
}

Chunk* ChunkStore::acquire() {

	if (_spare.empty()) {
		void* slab = _aligned_malloc(CHUNK_SLAB_SIZE, CHUNK_SIZE);
		if (slab == nullptr) return nullptr;

		_slabs.push_back(slab);

		unsigned char* base = static_cast<unsigned char*>(slab);
		for (size_t offset = 0; offset < CHUNK_SLAB_SIZE; offset += CHUNK_SIZE)
			_spare.push_back(reinterpret_cast<Chunk*>(base + offset));
	}

	Chunk* chunk = _spare.back();
	_spare.pop_back();
	return chunk;
}

void ChunkStore::release(Chunk* chunk) {
	_spare.push_back(chunk);
}


FixedAllocator::FixedAllocator(const size_t blockSize, const unsigned char sizeClass, ChunkStore* store) {
	_blockSize = blockSize;
	_sizeClass = sizeClass;
	_store = store;
}

void* FixedAllocator::Allocate() {

	if (_lastDeallocationChunk != nullptr) {
		void* allocation = _lastDeallocationChunk->Allocate(_blockSize);
		if (allocation != nullptr) {
			if (_lastDeallocationChunk->isFull())
				dropFromFreeChunks(_lastDeallocationChunk);
			return allocation;
		}
	}

	if (_lastAllocatedChunk != nullptr) {
		void* allocation = _lastAllocatedChunk->Allocate(_blockSize);
		if (allocation != nullptr) {
			if (_lastAllocatedChunk->isFull())
				dropFromFreeChunks(_lastAllocatedChunk);
			return allocation;
		}
	}

	while (!_freeChunks.empty()) {
		Chunk* candidate = _freeChunks.back();
		void* allocation = candidate->Allocate(_blockSize);

		if (allocation == nullptr || candidate->isFull())
			_freeChunks.pop_back();

		if (allocation != nullptr) {
			_lastAllocatedChunk = candidate;
			return allocation;
		}
	}

	void* memory = _store->acquire();
	if (memory == nullptr) return nullptr;

	Chunk* chunk = Chunk::Init(memory, _blockSize, _sizeClass);
	_lastAllocatedChunk = chunk;
	_lastDeallocationChunk = nullptr;

	void* allocation = chunk->Allocate(_blockSize);
	if (!chunk->isFull())
		_freeChunks.push_back(chunk);

	return allocation;
}

void FixedAllocator::Deallocate(void* ptr, Chunk* chunk) {

	const bool wasFull = chunk->isFull();

	chunk->Deallocate(ptr, _blockSize);

	_lastDeallocationChunk = chunk;

	if (wasFull) {
		_freeChunks.push_back(chunk);
	}
}

void FixedAllocator::dropFromFreeChunks(Chunk* chunk) {
	_freeChunks.erase(std::remove(_freeChunks.begin(), _freeChunks.end(), chunk), _freeChunks.end());
}


SmallObjAllocator::SmallObjAllocator(size_t smallObjectAllocation) {

	const unsigned int nearestExponent = getIndex(smallObjectAllocation);

	for (unsigned int i = 0; i <= nearestExponent; ++i) {
		const size_t blockSize = static_cast<size_t>(1) << i;
		_pool.push_back(new FixedAllocator(blockSize, static_cast<unsigned char>(i), &_store));
	}
}

SmallObjAllocator::~SmallObjAllocator() {
	for (FixedAllocator* fa : _pool) delete fa;
}

void* SmallObjAllocator::Allocate(size_t numBytes) {
	return _pool[getIndex(numBytes)]->Allocate();
}

void SmallObjAllocator::Deallocate(void* ptr) const {

	Chunk* chunk = Chunk::Owner(ptr);
	const unsigned char sizeClass = chunk->sizeClass();

	assert(sizeClass < _pool.size());

	_pool[sizeClass]->Deallocate(ptr, chunk);
}

unsigned int SmallObjAllocator::getIndex(size_t size) {

	if (size <= 1) return 0;

	unsigned long nearestExponent;
#ifdef _WIN64
	_BitScanReverse64(&nearestExponent, static_cast<unsigned __int64>(size - 1));
#else
	_BitScanReverse(&nearestExponent, static_cast<unsigned long>(size - 1));
#endif

	return static_cast<unsigned int>(nearestExponent) + 1u;
}
