#include "FixedAllocator.h"

#include <cassert>

namespace small_object_allocator
{
	FixedAllocator::FixedAllocator(std::size_t blockSize): blockSize(blockSize)
	{
		assert(blockSize > 0);

		prev = next = this;

		std::size_t numBlocks = DEFAULT_CHUNK_SIZE / blockSize;
		if (numBlocks > UCHAR_MAX) numBlocks = UCHAR_MAX;
		if (numBlocks == 0) numBlocks = 8 * blockSize; // i have no idea why he does this....

		blockCount = static_cast<unsigned char>(numBlocks);
		assert(blockCount == numBlocks); //truncation check

		lastAllocChunk = lastDeallocChunk = nullptr;
	}

	FixedAllocator::FixedAllocator(const FixedAllocator& rhs):
	blockSize(rhs.blockSize),
	blockCount(rhs.blockCount),
	chunks(rhs.chunks)
	{
		prev = &rhs;
		next = rhs.next;
		rhs.next->prev = this;
		rhs.next = this;

		lastAllocChunk = rhs.lastAllocChunk ? 
			&chunks.front() + (rhs.lastAllocChunk - &rhs.chunks.front()) : nullptr;
		lastDeallocChunk = rhs.lastDeallocChunk ? 
			&chunks.front() + (rhs.lastDeallocChunk - &rhs.chunks.front()) : nullptr;
	}

	FixedAllocator::FixedAllocator(FixedAllocator&& rhs) :
		blockSize(std::move(rhs.blockSize)),
		blockCount(std::move(rhs.blockCount)),
		chunks(std::move(rhs.chunks))
	{
		prev = std::move(rhs.prev);
		prev->next = this;
		next = std::move(rhs.next);
		next->prev = this;
		lastAllocChunk = std::move(rhs.lastAllocChunk);
		lastDeallocChunk = std::move(rhs.lastDeallocChunk);
	}

	FixedAllocator& FixedAllocator::operator=(const FixedAllocator& rhs)
	{
		FixedAllocator copy(rhs);

		std::swap(blockSize, copy.blockSize);
		std::swap(blockCount, copy.blockCount);
		chunks.swap(copy.chunks);
		std::swap(lastAllocChunk, copy.lastAllocChunk);
		std::swap(lastDeallocChunk, copy.lastDeallocChunk);

		return *this;
	}

	FixedAllocator& FixedAllocator::operator=(FixedAllocator&& rhs)
	{
		blockSize = std::move(rhs.blockSize);
		blockCount = std::move(rhs.blockCount);
		chunks = std::move(rhs.chunks);
		lastAllocChunk = std::move(rhs.lastAllocChunk);
		lastDeallocChunk = std::move(rhs.lastDeallocChunk);
		return *this;
	}

	FixedAllocator::~FixedAllocator()
	{
		if (chunks.size() == 0) return;
		if (blockSize > DEFAULT_CHUNK_SIZE) return;
		if (prev != this)
		{
			prev->next = next;
			next->prev = prev;
			return;
		}

		assert(prev == next);

		std::vector<Chunk>::iterator it = chunks.begin();
		for (;it != chunks.end();++it)
		{
			assert(it->blocksAvailable == blockCount); //imagine trying to destroy non empty chunks
			it->Destroy();
		}
	}

	void* FixedAllocator::Allocate()
	{
		//when no avail blocks, find a new one in a different chunk, or create a new chunk
		if (lastAllocChunk == 0 || lastAllocChunk->blocksAvailable == 0)
		{
			std::vector<Chunk>::iterator it = chunks.begin();
			for(;;++it)
			{
				if (it == chunks.end()) // no chunks had a free block, create a new one
				{
					chunks.reserve(chunks.size() + 1);
					chunks.emplace_back();
					lastAllocChunk = &chunks.back();
					lastDeallocChunk = lastAllocChunk;

					lastAllocChunk->Init(blockSize, blockCount);
					break;
				}
				else if (it->blocksAvailable > 0)
				{
					lastAllocChunk = &*it;
					break;
				}
			}
		}

		assert(lastAllocChunk != 0);
		assert(lastAllocChunk->blocksAvailable > 0);
		return lastAllocChunk->Allocate(blockSize);
	}

	void FixedAllocator::Deallocate(void* p)
	{
		std::size_t chunkUpperEnd = blockSize * blockCount;

		if (p < lastDeallocChunk->pData || p > lastDeallocChunk->pData + chunkUpperEnd)
		{
			Chunk* lo = lastDeallocChunk - 1;
			Chunk* hi = lastDeallocChunk + 1;
			Chunk* loBound = &chunks.front();
			Chunk* hiBound = &chunks.back();

			if (lo < loBound) lo = 0;
			if (hi > hiBound) hi = 0;

			for(;hi != 0 || lo != 0;)
			{
				if (lo)
				{
					if (p >= lo->pData && p < lo->pData + chunkUpperEnd)
					{
						lastDeallocChunk = lo;
						break;
					}
					if (--lo < loBound) lo = 0;
				}
				if (hi)
				{
					if (p >= hi->pData && p < hi->pData + chunkUpperEnd)
					{
						lastDeallocChunk = hi;
						break;
					}
					if (++hi > hiBound) hi = 0;
				}
			}
			assert(p >= lastDeallocChunk->pData && p < lastDeallocChunk->pData + chunkUpperEnd);
		}


		lastDeallocChunk->Deallocate(p, blockSize);

		if (lastDeallocChunk->blocksAvailable == blockCount && lastDeallocChunk != &chunks.back())
		{
			if (chunks.back().blocksAvailable == blockCount)
			{
				chunks.pop_back();
			}
			else
			{
				std::swap(*lastDeallocChunk, chunks.back());
			}
			lastDeallocChunk = &chunks.back();
		}

		lastAllocChunk = lastDeallocChunk;
	}
}
