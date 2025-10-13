#include "SmallObjectAllocator.h"
#include <exception>
#include <cassert>
#include <type_traits>

Chunk::Chunk(const size_t blockSize, const unsigned char blocks) {

	_firstAvailableBlock = 0;
	_blocksAvailable = blocks;

	_dataPtr = static_cast<unsigned char*>(malloc(blockSize * blocks));

	unsigned char* ptr = _dataPtr;
	for (unsigned char i = 0; i < blocks; ptr += blockSize) {
		*ptr = ++i;
	}

}

void Chunk::Release() {
	assert(_dataPtr != nullptr);
	free(_dataPtr);
}

void* Chunk::Allocate(const size_t blockSize) {

	if (_blocksAvailable <= 0)	return nullptr;

	--_blocksAvailable;

	unsigned char* currentBlock = _dataPtr + _firstAvailableBlock * blockSize;

	_firstAvailableBlock = *currentBlock;

	return currentBlock;
}

void Chunk::Deallocate(void* ptr, const size_t blockSize) {
	++_blocksAvailable;

	unsigned char* ptrInChar = (unsigned char*)ptr;

	*ptrInChar = _firstAvailableBlock;

	_firstAvailableBlock = (unsigned char)((ptrInChar - _dataPtr) / blockSize);

}

unsigned char* Chunk::Start() const {
	return _dataPtr;
}

bool Chunk::isEmpty(const unsigned char blocks) const {
	return _blocksAvailable == blocks;
}

bool Chunk::isFull() const {
	return _blocksAvailable == 0;
}

FixedAllocator::FixedAllocator(const size_t blockSize, const unsigned char blockPerChunk) {

	_blockSize = blockSize;
	AddChunk(blockPerChunk);
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
		if (_lastAllocatedChunk->isFull())
			_freeChunks.erase(std::remove(_freeChunks.begin(), _freeChunks.end(), _lastAllocatedChunk), _freeChunks.end());
		return allocation;
	}


	if (_freeChunks.size() > 0) {
		void* allocation = _freeChunks[0]->Allocate(_blockSize);

		if (_freeChunks[0]->isFull()) {
			_freeChunks.erase(_freeChunks.begin());
		}
		else {
			_lastAllocatedChunk = _freeChunks[0];
		}

		if (allocation != nullptr) {
			return allocation;
		}

	}
	AddChunk(blockPerChunk);
	return _lastAllocatedChunk->Allocate(_blockSize);
}

bool FixedAllocator::Deallocate(void* ptr, const unsigned char blockPerChunk) {
	unsigned char i = 0;
	bool wasFull = false;

	if (_lastDeallocationChunk != nullptr) {
		if (_lastDeallocationChunk->Start() < ptr && _lastDeallocationChunk->Start() + (_blockSize * blockPerChunk) >= ptr) {
			DoDeallocate(_lastDeallocationChunk, ptr, blockPerChunk);
			return true;
		}
	}

	auto currentNode = _chunks.getRoot();

	while (currentNode != nullptr) {

		if (currentNode->getData() < ptr && (unsigned char*)currentNode->getData() + (_blockSize * blockPerChunk) >= ptr) break;
		if (!currentNode->hasChilds())
			return false;

		NodePosition currentNodePosition = (NodePosition)(ptr >= currentNode->getData());
		currentNode = currentNode->childs[currentNodePosition];
	}

	Chunk* chunk = currentNode->getOtherDataPointer();
	DoDeallocate(chunk, ptr, blockPerChunk);
	return true;
}

void FixedAllocator::AddChunk(const unsigned char blockPerChunk) {
	Chunk newChunk(_blockSize, blockPerChunk);

	Chunk* chunk = _chunks.insert(newChunk.Start(), newChunk)->getOtherDataPointer();
	_lastDeallocationChunk = nullptr;
	_lastAllocatedChunk = chunk;
	_freeChunks.push_back(chunk);
}

void FixedAllocator::DoDeallocate(Chunk* chunk, void* ptr, const unsigned char blockPerChunk, int i) {

	bool wasFull = chunk->isFull();

	chunk->Deallocate(ptr, _blockSize);

	_lastDeallocationChunk = chunk;

	if (chunk->isEmpty(blockPerChunk) && _chunks.getRoot() != nullptr && _chunks.getRoot()->hasChilds()) {
		_chunks.remove(chunk->Start());
		chunk->Release();
		delete chunk;
		if (_lastAllocatedChunk == _lastDeallocationChunk)
			_lastAllocatedChunk = _chunks.getRoot()->getOtherDataPointer();
		_lastDeallocationChunk = nullptr;
	}
	else if (wasFull) {
		_freeChunks.push_back(chunk);
		_lastAllocatedChunk = nullptr;
	}
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

	double logElement = log2((double)size);

	unsigned int nearestExponent = trunc(logElement) == logElement ? (unsigned int)logElement : (unsigned int)logElement + 1u;

	return nearestExponent;
}