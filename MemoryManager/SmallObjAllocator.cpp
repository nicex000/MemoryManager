#include "SmallObjAllocator.h"

#include <cassert>

namespace small_object_allocator
{
	SmallObjAllocator::SmallObjAllocator(std::size_t chunkSize, std::size_t maxObjectSize) :
		defaultChunkSize(chunkSize), maxObjectSize(maxObjectSize)
	{
		lastAlloc = lastDealloc = nullptr;
	}

	void* SmallObjAllocator::Allocate(std::size_t numBytes)
	{
		//if (numBytes == 0) numBytes = defaultChunkSize;
		if (numBytes > maxObjectSize) return malloc(numBytes);

		if (lastAlloc && lastAlloc->BlockSize() == numBytes) return lastAlloc->Allocate();

		//finds the smallest closest fixed allocator
		//std::vector<FixedAllocator>::iterator it = std::lower_bound(pool.begin(), pool.end(), numBytes);
		auto mit = map.find(numBytes);
		std::vector<FixedAllocator>::iterator it = pool.begin() + (mit != map.end() ? (*mit).second : 0);

		//either it's too big so lower bound found nothing, or what it found is the wrong size
		if (it == pool.end() || it->BlockSize() != numBytes)
		{
			//add a new one right after current iterator pos
			//it = pool.insert(it, FixedAllocator(numBytes));
			pool.push_back(numBytes);
			it = pool.end() - 1;
			map.insert_or_assign(numBytes, pool.size() - 1);

		}
		lastAlloc = &*it;
		return lastAlloc->Allocate();
	}
	void SmallObjAllocator::Deallocate(void* p, std::size_t numBytes)
	{
		assert(numBytes);
		if (numBytes > maxObjectSize)
		{
			free(p);
			return;
		}

		if (lastDealloc && lastDealloc->BlockSize() == numBytes)
		{
			lastDealloc->Deallocate(p);
			return;
		}

		//finds the smallest closest fixed allocator
		//gstd::vector<FixedAllocator>::iterator it = std::lower_bound(pool.begin(), pool.end(), numBytes);
		auto mit = map.find(numBytes);
		std::vector<FixedAllocator>::iterator it = pool.begin() + (mit != map.end() ? (*mit).second : 0);

		//make sure that it found a legit one
		assert(it != pool.end() && it->BlockSize() == numBytes);

		lastDealloc = &*it;
		lastDealloc->Deallocate(p);
	}
}
