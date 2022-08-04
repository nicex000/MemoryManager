// MemoryManager.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#pragma once


#include "Tests.h"


int main()
{
    TimeTest(DefaultAllocatorFIFO, "DefaultAllocator FIFO");
    TimeTest(SmallObjectAllocatorFIFO, "SmallObjectAllocator FIFO");
    TimeTest(SmallObjectObjectTest, "SmallObject Object FIFO");
    TimeTest(GenericObjectAllocatorFIFO, "GenericObjectAllocator FIFO");
    std::cout << std::endl << std::endl;

    TimeTest(DefaultAllocatorFILO, "DefaultAllocator FILO");
    TimeTest(SmallObjectAllocatorFILO, "SmallObjectAllocator FILO");
    TimeTest(GenericObjectAllocatorFILO, "GenericObjectAllocator FILO");
    std::cout << std::endl << std::endl;

    TimeTest(DefaultAllocatorButterfly, "DefaultAllocator Butterfly");
    TimeTest(SmallObjectAllocatorButterfly, "SmallObjectAllocator Butterfly");
    TimeTest(GenericObjectAllocatorButterfly, "GenericObjectAllocator Butterfly");
    std::cout << std::endl << std::endl;

    TimeTest(DefaultAllocatorSmallArray, "DefaultAllocator SmallArray");
    TimeTest(SmallObjectAllocatorSmallArray, "SmallObjectAllocator SmallArray");
    TimeTest(GenericObjectAllocatorSmallArray, "GenericObjectAllocator SmallArray");
    std::cout << std::endl << std::endl;

    TimeTest(DefaultAllocatorLargeArray, "DefaultAllocator LargeArray");
    TimeTest(SmallObjectAllocatorLargeArray, "SmallObjectAllocator LargeArray");
    TimeTest(GenericObjectAllocatorLargeArray, "GenericObjectAllocator LargeArray");
    std::cout << std::endl << std::endl;


    TimeTest(STLSmallObjAllocator, "STL::SmallObjectAllocator");


    

    return 0;

}
