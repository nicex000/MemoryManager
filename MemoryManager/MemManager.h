#pragma once

#include "MemControlBlock.h"
#include "SmallObjAllocator.h"




#ifdef CUSTOM_MEM_MANAGER_V1

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

#endif // CUSTOM_MEM_MANAGER_V1

#ifdef CUSTOM_MEM_MANAGER_V2

inline small_object_allocator::SmallObjAllocator& GetSmallMemoryAllocator()
{
    static small_object_allocator::SmallObjAllocator single =
        small_object_allocator::SmallObjAllocator(DEFAULT_CHUNK_SIZE, 64);
        
    return single;
}

inline MemControlBlockAllocator& GetGenericAllocator()
{
    static MemControlBlockAllocator single(209715200);
    return single;
}

template<typename T>
void* SM_NEW()
{
    return GetSmallMemoryAllocator().Allocate(sizeof(T));
}

template<typename T, std::size_t length>
void* SM_NEW_A()
{
    return GetSmallMemoryAllocator().Allocate(sizeof(T) * length);
}

template<typename T>
void SM_DELETE(T* p)
{
    p->~T();
    GetSmallMemoryAllocator().Deallocate(p, sizeof(T));
}

template<typename T>
void SM_DELETE_A(T* p, std::size_t length)
{
    for(std::size_t i = 0; i < length; ++i)
    {
        p[i].~T();
    }
    GetSmallMemoryAllocator().Deallocate(p, sizeof(T)*length);
}



template<typename T>
void* GM_NEW()
{
    return GetGenericAllocator().Allocate(sizeof(T));
}

template<typename T, std::size_t length>
void* GM_NEW_A()
{
    return GetGenericAllocator().Allocate(sizeof(T) * length);
}

template<typename T>
void GM_DELETE(T* p)
{
    p->~T();
    GetGenericAllocator().Deallocate(p);
}

template<typename T>
void GM_DELETE_A(T* p, std::size_t length)
{
    for (std::size_t i = 0; i < length; ++i)
    {
        p[i].~T();
    }
    GetGenericAllocator().Deallocate(p);
}







/*
void* operator new(std::size_t sz)
{
    return SM_NEW(sz);
}

void* operator new[](std::size_t sz)
{
    return SM_NEW_A(sz);
}
void operator delete(void* p, std::size_t sz)
{
    SM_DELETE(p, sz);
}
void operator delete[](void* p, std::size_t sz)
{
    SM_DELETE_A(p, sz);
}
 */

#endif // CUSTOM_MEM_MANAGER_V2
