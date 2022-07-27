#pragma once

#include <random>

#include <iostream>
#include <iomanip>
#include <chrono>

#include "MemManager.h"

#include "ComplexTypeA.h"
//#include "ComplexTypeB.h"
//#include "ComplexTypeC.h"
//#include "ComplexTypeD.h"
//#include "ComplexTypeE.h"


#define ITERATIONS 100
#define ARRAY_SIZE 1000


inline void TimeTest(void (*test)(), std::string name)
{
    const auto start = std::chrono::system_clock::now();

    test();

    const auto end = std::chrono::system_clock::now();

    const std::chrono::duration<double> diff = end - start;
    std::cout << name << std::setw(9)
        << diff.count() << " s\n";
}


inline void DefaultAllocatorFIFO()
{
    Complex* array[ARRAY_SIZE];
    for (int i = 0; i < ITERATIONS; ++i) {
        for (int j = 0; j < ARRAY_SIZE; ++j) {
            //std::cout << i << " - " << j << std::endl;
            array[j] = new Complex(i, j);
        }
        for (int j = 0; j < ARRAY_SIZE; ++j) {
            delete array[j];
        }
    }
}

inline void DefaultAllocatorFILO()
{
    Complex* array[ARRAY_SIZE];
    for (int i = 0; i < ITERATIONS; ++i) {
        for (int j = 0; j < ARRAY_SIZE; ++j) {
            array[j] = new Complex(i, j);
        }
        for (int j = ARRAY_SIZE-1; j >= 0; --j) {
            delete array[j];
        }
    }
}

inline void DefaultAllocatorButterfly()
{
    Complex* array[ARRAY_SIZE];
    for (int i = 0; i < ARRAY_SIZE; i++)
    {
        array[i] = nullptr;
    }

    std::random_device rnd;
    std::mt19937 mt(rnd());
    std::uniform_int_distribution<> randARRAY_SIZE(0, ARRAY_SIZE-1);
    int rand;
    for (int i = 0; i < ITERATIONS; ++i) {
        for (int j = 0; j < ARRAY_SIZE; ++j) {
            rand = randARRAY_SIZE(mt);

            if (array[rand] != nullptr)
            {
                delete array[rand];
                array[rand] = nullptr;
            }
            else
            {
                array[rand] = new Complex(i, j);
            }
        }
    }
    for (int i = 0; i < ARRAY_SIZE; ++i)
    {
        if (array[i] != nullptr)
            delete array[i];
    }
}

inline void DefaultAllocatorSmallArray()
{
    Complex* array = new Complex[3]{ Complex(1,2), Complex(3,4), Complex(5,6) };
    delete[] array;
}

inline void DefaultAllocatorLargeArray()
{
    int* array = new int[ARRAY_SIZE]();
    delete[] array;
}

inline void SmallObjectAllocatorFIFO()
{
    Complex* array[ARRAY_SIZE];
    for (int i = 0; i < ITERATIONS; ++i) {
        for (int j = 0; j < ARRAY_SIZE; ++j) {
            //std::cout << i << " - " << j << std::endl;
            array[j] = new(SM_NEW<Complex>()) Complex(i, j);
        }
        for (int j = 0; j < ARRAY_SIZE; ++j) {
            SM_DELETE(array[j]);
        }
    }
}

inline void SmallObjectAllocatorFILO()
{
    Complex* array[ARRAY_SIZE];
    for (int i = 0; i < ITERATIONS; ++i) {
        for (int j = 0; j < ARRAY_SIZE; ++j) {
            //std::cout << i << " - " << j << std::endl;
            array[j] = new(SM_NEW<Complex>()) Complex(i, j);
        }
        for (int j = ARRAY_SIZE-1; j >= 0; --j) {
            SM_DELETE(array[j]);
        }
    }
}

inline void SmallObjectAllocatorButterfly()
{
    Complex* array[ARRAY_SIZE];
    for (int i = 0; i < ARRAY_SIZE; i++)
    {
        array[i] = nullptr;
    }

    std::random_device rnd;
    std::mt19937 mt(rnd());
    std::uniform_int_distribution<> randARRAY_SIZE(0, ARRAY_SIZE-1);
    int rand;
    for (int i = 0; i < ITERATIONS; ++i) {
        for (int j = 0; j < ARRAY_SIZE; ++j) {
            rand = randARRAY_SIZE(mt);

            if (array[rand] != nullptr)
            {
                SM_DELETE(array[rand]);
                array[rand] = nullptr;
            }
            else
            {
                array[rand] = new(SM_NEW<Complex>()) Complex(i, j);
            }
        }
    }
    for (int i = 0; i < ARRAY_SIZE; ++i)
    {
        if (array[i] != nullptr)
            SM_DELETE(array[i]);
    }
}

inline void SmallObjectAllocatorSmallArray()
{
    Complex* array = new(SM_NEW_A<Complex, 3>()) Complex[3]{ Complex(1,2), Complex(3,4), Complex(5,6) };
    SM_DELETE_A(array, 3);
}

inline void SmallObjectAllocatorLargeArray()
{
    //size is too big for small obj allocator so it will use the fallback internally to malloc/free
    int* array = new(SM_NEW_A<int, ARRAY_SIZE>()) int[ARRAY_SIZE]();
    SM_DELETE_A(array, ARRAY_SIZE);
}


class SmallObject
{
public:
    SmallObject(double a, double b) :r(a), c(b){};
    static void* operator new(std::size_t sz);
    static void operator delete(void* p);
    void DoStuff();
private:
    double r;
    double c;
};

inline void* SmallObject::operator new(std::size_t sz)
{
    return GetSmallMemoryAllocator().Allocate(sizeof(SmallObject));
}

inline void SmallObject::operator delete(void* p)
{
    GetSmallMemoryAllocator().Deallocate(p, sizeof(SmallObject));
}

inline void SmallObject::DoStuff()
{
    int* thing = new int;
    delete thing;
}

inline void SmallObjectObjectTest()
{
    SmallObject* soArray[ARRAY_SIZE];
    for (int i = 0; i < ITERATIONS; ++i) {
        for (int j = 0; j < ARRAY_SIZE; ++j) {
            soArray[j] = new SmallObject(i,j);
            //soArray[j]->DoStuff();
        }
        for (int j = 0; j < ARRAY_SIZE; ++j) {
            delete soArray[j];
        }
    }
}


inline void GenericObjectAllocatorFIFO()
{
    Complex* array[ARRAY_SIZE];
    for (int i = 0; i < ITERATIONS; ++i) {
        for (int j = 0; j < ARRAY_SIZE; ++j) {
            //std::cout << i << " - " << j << std::endl;
            array[j] = new(GM_NEW<Complex>()) Complex(i, j);
        }
        for (int j = 0; j < ARRAY_SIZE; ++j) {
            GM_DELETE(array[j]);
        }
    }
}

inline void GenericObjectAllocatorFILO()
{
    Complex* array[ARRAY_SIZE];
    for (int i = 0; i < ITERATIONS; ++i) {
        for (int j = 0; j < ARRAY_SIZE; ++j) {
            //std::cout << i << " - " << j << std::endl;
            array[j] = new(GM_NEW<Complex>()) Complex(i, j);
        }
        for (int j = ARRAY_SIZE-1; j >= 0; --j) {
            GM_DELETE(array[j]);
        }
    }
}

inline void GenericObjectAllocatorButterfly()
{
    Complex* array[ARRAY_SIZE];
    for (int i = 0; i < ARRAY_SIZE; i++)
    {
        array[i] = nullptr;
    }

    std::random_device rnd;
    std::mt19937 mt(rnd());
    std::uniform_int_distribution<> randARRAY_SIZE(0, ARRAY_SIZE-1);
    int rand;
    for (int i = 0; i < ITERATIONS; ++i) {
        for (int j = 0; j < ARRAY_SIZE; ++j) {
            rand = randARRAY_SIZE(mt);

            if (array[rand] != nullptr)
            {
                GM_DELETE(array[rand]);
                array[rand] = nullptr;
            }
            else
            {
                array[rand] = new(GM_NEW<Complex>()) Complex(i, j);
            }
        }
    }
    for (int i = 0; i < ARRAY_SIZE; ++i)
    {
        if (array[i] != nullptr)
            GM_DELETE(array[i]);
    }
}

inline void GenericObjectAllocatorSmallArray()
{
    Complex* array = new(GM_NEW_A<Complex, 3>()) Complex[3]{ Complex(1,2), Complex(3,4), Complex(5,6) };
    GM_DELETE_A(array, 3);
}

inline void GenericObjectAllocatorLargeArray()
{
    //size is too big for small obj allocator so it will use the fallback internally to malloc/free
    int* array = new(GM_NEW_A<int, ARRAY_SIZE>()) int[ARRAY_SIZE]();
    GM_DELETE_A(array, ARRAY_SIZE);
}