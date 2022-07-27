#include "Chunk.h"

#include <cassert>

namespace small_object_allocator
{
	void Chunk::Init(std::size_t blockSize, unsigned char blocks)
	{
		pData = static_cast<unsigned char*>(malloc(blockSize * blocks));
		firstAvailableBlock = 0;
		blocksAvailable = blocks;
		unsigned char num = firstAvailableBlock;
		unsigned char* it = pData;
		for (; num != blocks; it += blockSize)
		{
			*it = ++num; //pre-increment cause we are storing who the next avail is
		}
	}

	void* Chunk::Allocate(std::size_t blockSize)
	{
		if (!blocksAvailable) return 0;
		unsigned char* pResult = pData + (firstAvailableBlock * blockSize);

		firstAvailableBlock = *pResult;
		--blocksAvailable;

		return pResult;
	}

	void Chunk::Deallocate(void* p, std::size_t blockSize)
	{
		assert(p >= pData);
		unsigned char* pToRelease = static_cast<unsigned char*>(p);
		assert((pToRelease - pData) % blockSize == 0); // alignment check
		*pToRelease = firstAvailableBlock;
		firstAvailableBlock = static_cast<unsigned char>((pToRelease - pData) / blockSize);
		assert(firstAvailableBlock == (pToRelease - pData) / blockSize); //truncation check
		++blocksAvailable;
	}

	void Chunk::Destroy()
	{
		free(pData);
	}
}

