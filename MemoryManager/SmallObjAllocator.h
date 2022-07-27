#pragma once
#include <map>
#include <vector>

#include "FixedAllocator.h"


namespace small_object_allocator
{
	class SmallObjAllocator
	{
	public:
		SmallObjAllocator(std::size_t chunkSize, std::size_t maxObjectSize);
		void* Allocate(std::size_t numBytes = 0);
		void Deallocate(void* p, std::size_t numBytes= 0);
	private:
		std::vector<FixedAllocator> pool;
		FixedAllocator* lastAlloc;
		FixedAllocator* lastDealloc;
		std::size_t defaultChunkSize, maxObjectSize;
		std::map<std::size_t, std::size_t> map;

			void* operator new(std::size_t size) { return malloc(size); }
		void operator delete(void* p) { free(p); }

	};
}

