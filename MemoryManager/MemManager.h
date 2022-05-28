#pragma once
#include <iostream>
#include <vector>



class Complex
{
public:
    Complex(double a, double b) : r(a), c(b) {}
private:
    double r; // Real Part
    double c; // Complex Part
};


#ifdef CUSTOM_MEM_MANAGER

class IMemoryManager
{
public:
    virtual void* allocate(size_t) = 0;
    virtual void free(void*) = 0;
};

class MemoryManager : public IMemoryManager
{
    struct FreeStore
    {
        FreeStore* next;
    };
    void expandPoolSize();
    void cleanUp();
    FreeStore* freeStoreHead;
public:
    MemoryManager()
    {
        freeStoreHead = 0;
        expandPoolSize();
    }
    virtual ~MemoryManager()
    {
        cleanUp();
    }
    virtual void* allocate(size_t);
    virtual void free(void*);
};

MemoryManager gMemoryManager; // Memory Manager, global variable.

#define POOLSIZE 32

void MemoryManager::expandPoolSize()
{
    size_t size = (sizeof(Complex) > sizeof(FreeStore*)) ? sizeof(Complex) : sizeof(FreeStore*);
    FreeStore* head = reinterpret_cast<FreeStore*>(malloc(size));
    freeStoreHead = head;

    for (int i = 0; i < POOLSIZE; ++i)
    {
        head->next = reinterpret_cast<FreeStore*>(malloc(size));
        head = head->next;
    }
    head->next = 0;
}

void MemoryManager::cleanUp()
{
    FreeStore* nextPtr = freeStoreHead;
    for (; nextPtr; nextPtr = freeStoreHead)
    {
        freeStoreHead = freeStoreHead->next;
        delete[] nextPtr;
    }
}


inline void* MemoryManager::allocate(size_t size)
{
    if (freeStoreHead == 0)
        expandPoolSize();

    FreeStore* head = freeStoreHead;
    freeStoreHead = head->next;
    return head;
}

inline void MemoryManager::free(void* deleted)
{
    FreeStore* head = static_cast<FreeStore*>(deleted);
    head->next = freeStoreHead;
    freeStoreHead = head;
}



void* operator new(size_t size)
{
    //std::cout << "QQQQQQ";
    return gMemoryManager.allocate(size);;
}

void operator delete(void* ptr) 
{ 
    //std::cout << "EEEEEE";
    gMemoryManager.free(ptr);
}

void* operator new[](size_t size)
{
    //std::cout << "qarr";
    return gMemoryManager.allocate(size);;
}

void operator delete[](void* ptr)
{
    //std::cout << "earr";
    gMemoryManager.free(ptr);
}

#endif // CUSTOM_MEM_MANAGER






/*
void* MM_NEW(size_t size)
{
    return malloc(size);
}

void MM_DELETE(void* ptr)
{
    free(ptr);
}




















struct Chunk
{
    void Init(std::size_t blockSize, unsigned char blocks);
    void* Allocate(std::size_t blockSize);
    void Deallocate(void* p, std::size_t blockSize);
    unsigned char* pData_;
    unsigned char
        firstAvailableBlock_,
        blocksAvailable_;
};

class FixedAllocator
{
private:
    std::size_t blockSize_;
    unsigned char numBlocks_;
    typedef std::vector<Chunk> Chunks;
    Chunks chunks_;
    Chunk   * allocChunk_;
    Chunk* deallocChunk_;
};

class SmallObjAllocator
{
public:
    SmallObjAllocator(
        std::size_t chunkSize,
        std::size_t maxObjectSize);
    void* Allocate(std::size_t numBytes);
    void Deallocate(void* p, std::size_t size);
private:
    std::vector<FixedAllocator> pool_;
};*/