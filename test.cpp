#include <chrono>
#include <iostream>
#include <vector>
#include "MemoryManager.h"


class CustomObject CUSTOMMEMORY {

public:
	int a;
	int b;

private:
	int c;

public:
	CustomObject(int _a, int _b, int _c) : a(_a), b(_b), c(_c) {}

	int GetC() { return c; }
};


class BigObject {
public:
	char data[2000] = { 5 };
	BigObject() = default;
};

class CustomBigObject : public BigObject {
public:
	static void* operator new(size_t sz) {
		if (void* ptr = my_allocate(sz))
			return ptr;

		throw std::bad_alloc{};
	}
	static void* operator new[](size_t sz) { return operator new(sz); }
	static void operator delete(void* ptr) { my_deallocate(ptr); }
	static void operator delete[](void* ptr) {
		operator delete(ptr);
	}
};

class NormalObject {
public:
	int a;
	int b;

private:
	int c;

public:
	NormalObject(int _a, int _b, int _c) : a(_a), b(_b), c(_c) {}

	int GetC() { return c; }
};


void TestWithLocalSmallObject(int numIterazioni)
{
	auto start = std::chrono::high_resolution_clock::now();
	for (size_t i = 0; i < numIterazioni; i++)
	{
		CustomObject* t = new CustomObject(0, 0, 0);
		delete t;
	}
	auto end = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	std::cout << "SMALL OBJECT ALLOCATOR Tempo totale di " << numIterazioni << " alloc+dealloc : " << duration.count() / 1000.000 << " secondi" << std::endl;
}

void TestWithoutLocalSmallObject(int numIterazioni)
{

	auto start = std::chrono::high_resolution_clock::now();
	for (size_t i = 0; i < numIterazioni; i++)
	{
		NormalObject* t = new NormalObject(0, 0, 0);
		delete t;
	}
	auto end = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	std::cout << "allocatore di sistema Tempo totale di " << numIterazioni << " alloc+dealloc: " << duration.count() / 1000.000 << " secondi" << std::endl;
}

void BulkTestWithSmallObject(int iterations)
{

	std::vector<CustomObject*> objects;
	objects.reserve(iterations);

	auto start = std::chrono::high_resolution_clock::now();

	// Allocazione massiva
	for (int i = 0; i < iterations; ++i)
	{
		objects.push_back(new CustomObject(0, 0, 0));
	}

	// Deallocazione massiva
	for (auto* obj : objects)
	{
		delete obj;
	}

	auto end = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
	std::cout << "SMALL OBJECT ALLOCATOR  Alloc-Dealloc Massiva di " << iterations << " iterazioni: " << duration.count() / 1000000.000000 << " secondi" << std::endl;
}

void BulkTestWithSystem(int iterations)
{
	std::vector<NormalObject*> objects;
	objects.reserve(iterations);

	auto start = std::chrono::high_resolution_clock::now();

	// Allocazione massiva
	for (int i = 0; i < iterations; ++i)
	{
		objects.push_back(new NormalObject(0, 0, 0));
	}

	// Deallocazione massiva
	for (auto* obj : objects)
	{
		delete obj;
	}

	auto end = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
	std::cout << "allocatore di sistema  Alloc-Dealloc Massiva di " << iterations << " iterazioni : " << duration.count() / 1000000.000000 << " secondi" << std::endl;
}

void TestBig(int inte = 10) {
	std::vector<CustomBigObject*> objects;
	objects.reserve(inte);
	auto start = std::chrono::high_resolution_clock::now();

	for (int i = 0; i < inte; ++i) {
		objects.push_back(new CustomBigObject);
	}


	for (auto* obj : objects) {
		delete obj;
	}
	auto end = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
	std::cout << "allocatore custom Alloc-Dealloc Massiva di " << inte << " iterazioni e " << sizeof(BigObject) << " in size: " << duration.count() / 1000000.000000 << " secondi" << std::endl;
}

void TestBigNormal(int inte = 10) {
	std::vector<BigObject*> objects;
	objects.reserve(inte);
	auto start = std::chrono::high_resolution_clock::now();

	for (int i = 0; i < inte; ++i) {
		objects.push_back(new BigObject);
	}

	for (auto* obj : objects) {
		delete obj;
	}
	auto end = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
	std::cout << "allocatore di sistema Alloc-Dealloc Massiva di " << inte << " iterazioni e " << sizeof(BigObject) << " in size: " << duration.count() / 1000000.000000 << " secondi" << std::endl;
}


int _global_variable = 1;

int main()
{
	int ordine = 10 * 10;
	int numIterazioni = 1000 * ordine;
	//TestWithLocalSmallObject(numIterazioni);
	//TestWithoutLocalSmallObject(numIterazioni);

	//BulkTestWithSmallObject(numIterazioni);
	//BulkTestWithSystem(numIterazioni);
	//TestBig(numIterazioni);
	//TestBigNormal(numIterazioni);
}
