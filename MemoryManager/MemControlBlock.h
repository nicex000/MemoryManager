#pragma once
#include <cassert>
#include <vector>

struct MemControlBlock
{
	bool available_;
	size_t size;
	MemControlBlock* prev_;
	MemControlBlock* next_;
	MemControlBlock(bool avail, size_t size): available_(avail), size(size)
	{
		prev_ = next_ = nullptr;
	}
};

class MemControlBlockAllocator
{
	unsigned char* const memory;
	MemControlBlock* first = nullptr;
	MemControlBlock* last = nullptr;



	MemControlBlock* WorstFinder(std::size_t blockSize)
	{
		if (first->next_ == nullptr) return first;

		MemControlBlock* current = first;
		MemControlBlock* biggest = current;

		while (current != nullptr)
		{
			/*while (biggest != nullptr && !biggest->available_)
				biggest = biggest->next_;

			if (biggest == nullptr) return nullptr;

			if (current->available_ && blockSize <= current->size && current->size >= biggest->size)
				biggest = current;

			current = current->next_;*/

			if (current->available_ && blockSize <= current->size && current->size > biggest->size)
				biggest = current;
			current = current->next_;
		}

		if (!biggest->available_ || biggest->size < blockSize) 
			return nullptr;

		return biggest;
	}

	MemControlBlock* FirstFinder(std::size_t blockSize)
	{
		if (first->next_ == nullptr) return first;

		MemControlBlock* current = first;

		while (current != nullptr)
		{
			if (current->available_ && blockSize <= current->size )
				return current;
			current = current->next_;

		}

		return nullptr;
	}

	void* FragmentBlock(MemControlBlock* blockWithRoom, std::size_t size)
	{
		blockWithRoom->available_ = false;
		size_t memControlBlockSize = sizeof(MemControlBlock);
		size_t spareSize = blockWithRoom->size - size;

		if (spareSize >= memControlBlockSize)
		{
			//fagment time
			unsigned char* fragmentedAddrStart = reinterpret_cast<unsigned char*>(blockWithRoom);
			unsigned char* fragmentedAddr = fragmentedAddrStart + memControlBlockSize + size;
			size_t fragmentedSize = spareSize - memControlBlockSize;
			MemControlBlock* fragmentedBlock = new(fragmentedAddr) MemControlBlock(true, fragmentedSize);

			if (blockWithRoom->next_ != nullptr)
				blockWithRoom->next_->prev_ = fragmentedBlock;

			fragmentedBlock->prev_ = blockWithRoom;
			fragmentedBlock->next_ = blockWithRoom->next_;
			blockWithRoom->next_ = fragmentedBlock;
			blockWithRoom->size -= (fragmentedSize + memControlBlockSize);

			while (last->next_ != nullptr) last = last->next_;

			return fragmentedAddrStart + memControlBlockSize;
		}
		return nullptr;
	}

public:
	MemControlBlockAllocator(std::size_t blockSize) : memory(new unsigned char[blockSize])
	{
		first = new(memory) MemControlBlock(true, blockSize - sizeof(MemControlBlock));
		last = first;
	}
	MemControlBlockAllocator(const MemControlBlockAllocator&) = delete;
	MemControlBlockAllocator(MemControlBlockAllocator&&) = delete;
	MemControlBlockAllocator& operator = (const MemControlBlockAllocator&) = delete;
	MemControlBlockAllocator& operator = (MemControlBlockAllocator&&) = delete;
	~MemControlBlockAllocator()
	{
		delete[] memory;
	}

	void* Allocate(std::size_t size)
	{
		MemControlBlock* blockWithRoom = WorstFinder(size);

		assert(blockWithRoom);

		if (blockWithRoom->next_ == nullptr && size == blockWithRoom->size)
		{
			blockWithRoom->size += size;
			blockWithRoom->available_ = false;
			return (reinterpret_cast<unsigned char*>(blockWithRoom) + sizeof(MemControlBlock));
		}

		if (blockWithRoom == nullptr)
			return nullptr;

		return FragmentBlock(blockWithRoom, size);
		
	}

	bool Deallocate(void* p)
	{
		if (p < first || p > last + last->size) return false;
		unsigned char* controlBlockAddress = (static_cast<unsigned char*>(p) - sizeof(MemControlBlock));
		MemControlBlock* current = reinterpret_cast<MemControlBlock*>(controlBlockAddress);

		current->available_ = true;
		if (current->prev_ != nullptr && current->prev_->available_) {

			current->prev_->size = current->prev_->size + sizeof(MemControlBlock) + current->size;

			current->prev_->next_ = current->next_;
			if (current->next_ != nullptr) current->next_->prev_ = current->prev_;

		}

		if (current == last) last = last->prev_;

		if (current->next_ != nullptr && current->next_->available_) {
			if (current->prev_ != nullptr && current->prev_->available_)
			{
				
				current->prev_->size = current->prev_->size + sizeof(MemControlBlock) + current->next_->size;
				current->prev_->next_ = current->next_->next_;
				if (current->next_->next_)
					current->next_->next_->prev_ = current->prev_;
				else if (current->next_ == last) last = current->prev_;
				current->next_->next_ = nullptr;
				current->next_->prev_ = nullptr;

				current->prev_ = nullptr;
				current->next_ = nullptr;

			}
		}


		//current->~MemControlBlock();
		return true;
	}
};

class HeapAllocator
{
	std::vector<MemControlBlockAllocator> pool;
	std::size_t minBlockSize;

public:
	HeapAllocator(std::size_t minBlockSize, std::size_t initialPoolCount): minBlockSize(minBlockSize)
	{
		for (std::size_t i = 0; i < initialPoolCount; ++i)
		{
			//pool.emplace_back(MemControlBlockAllocator(minBlockSize));
		}
	}

	void* Allocate(std::size_t size)
	{
		void* addr = nullptr;
		std::vector<MemControlBlockAllocator>::iterator it = pool.begin();
		for (;; ++it)
		{
			if (it == pool.end()) break;

			addr = it->Allocate(size);
			if (addr != nullptr) break;
		}

		return addr;
	}

	void Deallocate(void* p)
	{
		std::vector<MemControlBlockAllocator>::iterator it = pool.begin();
		for (;; ++it)
		{
			if (it == pool.end()) break;
			if (it->Deallocate(p)) break;
		}
	}
};