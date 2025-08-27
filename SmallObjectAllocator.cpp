#include "SmallObjectAllocator.h"
#include <exception>
#include <cassert>

Chunk::Chunk(const size_t blockSize, const unsigned char blocks) {

	_firstAvailableBlock = 0;
	_blocksAvailable = blocks;

	_dataPtr = static_cast<unsigned char*>(::std::malloc (blockSize * blocks));

	for (unsigned char i = 0; i < blocks; ++i) {
		unsigned char* ptr = _dataPtr + (blockSize * (i));
		*ptr = i+1;
	}

}

void Chunk::Release() {
	assert(_dataPtr != nullptr);
	free(static_cast<void*>(_dataPtr));
}

void* Chunk::Allocate(const size_t blockSize) {

	if (_blocksAvailable <= 0)	return nullptr;

	--_blocksAvailable;

	void* currentBlock = _dataPtr + _firstAvailableBlock * blockSize;

	_firstAvailableBlock = *((unsigned char*) currentBlock);

	return currentBlock;
}

void Chunk::Deallocate(void* ptr, const size_t blockSize) {
	++_blocksAvailable;

	unsigned char* ptrInChar = (unsigned char*)ptr;

	_firstAvailableBlock = (unsigned char) ((ptrInChar - _dataPtr) / blockSize);

}

unsigned char* Chunk::Start() const{
	return _dataPtr;
}

bool Chunk::isEmpty(const unsigned char blocks) const {
	return _blocksAvailable == blocks;
}

FixedAllocator::FixedAllocator(const size_t blockSize, const unsigned char blockPerChunk) {

	_blockSize = blockSize;
	Chunk newChunk(_blockSize, blockPerChunk);
	_chunks.push_back(newChunk);
	_lastAllocatedChunk = &_chunks.front();
}

FixedAllocator::~FixedAllocator() {
	free(_lastAllocatedChunk);
	free(_lastDeallocationChunk);
}

void* FixedAllocator::Allocate(const unsigned char blockPerChunk) {
	//try to fill the last chunk that recive a deallocation
	if (_lastDeallocationChunk != nullptr) {
		void* allocation = _lastDeallocationChunk->Allocate(_blockSize);
		if (allocation != nullptr) {
			return allocation;
		}
	}

	//try to fill the last chunk that was allocated
	void* allocation = _lastAllocatedChunk->Allocate(_blockSize);
	if (allocation != nullptr) {
		return allocation;
	}

	for (auto riter = _chunks.rbegin();
		riter != _chunks.rend(); ++riter) {
		auto chunk = *riter;
		if (&chunk == _lastDeallocationChunk || &chunk == _lastAllocatedChunk) continue;

		void* allocation = chunk.Allocate(_blockSize);

		if (allocation != nullptr) {
			return allocation;
		}
	}

	//check if a chunk is free
	/*for (Chunk chunk : _chunks) {
		if (&chunk == _lastDeallocationChunk || &chunk == _lastAllocatedChunk) continue;

		void* allocation = chunk.Allocate(_blockSize);

		if (allocation != nullptr) {
			return allocation;
		}

	}*/

	//create new chunk
	Chunk newChunk(_blockSize, blockPerChunk);
	_chunks.push_back(newChunk);
	_lastAllocatedChunk = &_chunks.back();

	return _lastAllocatedChunk->Allocate(_blockSize);
}

bool FixedAllocator::Deallocate(void* ptr, const unsigned char blockPerChunk) {
	unsigned char i = 0;
	for (unsigned char i = 0; i < _chunks.size(); ++i) {
		Chunk* chunk = &_chunks[i];
;
		if (chunk->Start() > ptr || chunk->Start() + (_blockSize * blockPerChunk) < ptr) continue;

		chunk->Deallocate(ptr, _blockSize);

		_lastDeallocationChunk = chunk;

		if (chunk->isEmpty(blockPerChunk) && _chunks.size() != 1) {
			_chunks.erase(_chunks.begin() + i);
			chunk->Release();
			_lastAllocatedChunk = &_chunks.back();
			_lastDeallocationChunk = nullptr;
		}
		return true;
		
	}
	return false;
}


size_t power(int a, int b) {
	size_t result = 1;
	for (int i = 0; i < b; ++i) result *= a;
	return result;
}

SmallObjAllocator::SmallObjAllocator(size_t smallObjectAllocation) {

	int nearestExponent = getIndex(smallObjectAllocation);

	for (int i = 0; i <= nearestExponent; ++i) {
		size_t blockSize = power(2, i);
		_pool.push_back(new FixedAllocator(blockSize, _blockPerChunk));
	}
}

void* SmallObjAllocator::Allocate(size_t numBytes) {
	return _pool[getIndex(numBytes)]->Allocate(_blockPerChunk);
}

void SmallObjAllocator::Deallocate(void* ptr) {
	for (FixedAllocator* fa : _pool) {
		if (fa->Deallocate(ptr, _blockPerChunk)) break;
	}
}


int SmallObjAllocator::getIndex(size_t size) {

	double logElement = log2((double) size);

	unsigned int nearestExponent = trunc(logElement) == logElement ? (unsigned int)logElement : (unsigned int)logElement + 1u;

	return nearestExponent;
}