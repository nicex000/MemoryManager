#pragma once
#include <iostream>
namespace small_object_allocator
{
	struct Chunk
	{
		void Init(std::size_t blockSize, unsigned char blocks);
		void* Allocate(std::size_t blockSize);
		void Deallocate(void* p, std::size_t blockSize);
		void Destroy();
		unsigned char* pData;
		unsigned char firstAvailableBlock;
		unsigned char blocksAvailable;
	};
}

