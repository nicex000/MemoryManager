#pragma once
#include <vector>

#include "Chunk.h"

//#define UCHAR_MAX 255

#ifndef DEFAULT_CHUNK_SIZE
#define DEFAULT_CHUNK_SIZE 4096 // how? 4kb is our fixed allocator's chunk size upper limit.
#endif
namespace small_object_allocator
{
	class FixedAllocator
	{
		std::size_t blockSize;
		unsigned char blockCount;
		std::vector<Chunk> chunks;
		Chunk* lastAllocChunk;
		Chunk* lastDeallocChunk;


		// because it tries to delete the data (through destructor) every time you do insert/push_back in the pool
		// i have no idea if this fixed it or if changing from c++14 to c++20 fixed it
		mutable const FixedAllocator* prev;
		mutable const FixedAllocator* next;
	public:
		FixedAllocator(std::size_t blockSize = 0);
		FixedAllocator(const FixedAllocator& rhs);
		FixedAllocator(FixedAllocator&& rhs);
		FixedAllocator& operator=(const FixedAllocator& rhs);
		FixedAllocator& operator=(FixedAllocator&& rhs);

		~FixedAllocator();
		void* Allocate();
		void Deallocate(void* p);
		std::size_t BlockSize() const { return blockSize; }
		bool operator <(const std::size_t rhs) const { return blockSize < rhs; } //need this for the compare on smallojb allocate/deallocate
		void* operator new(std::size_t size) { return malloc(size); }
		void operator delete(void* p) { free(p); }
	};
}
