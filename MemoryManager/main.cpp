// MemoryManager.cpp : This file contains the 'main' function. Program execution begins and ends there.
//


#include <iostream>
#include <iomanip>
#include <vector>
#include <numeric>
#include <chrono>
#include "MemManager.h"

int main()
{
    auto start = std::chrono::system_clock::now();
    Complex* array[1000];
    for (int i = 0; i < 5000; i++) {
        for (int j = 0; j < 1000; j++) {
            //std::cout << i << " - " << j << std::endl;
            array[j] = new Complex(i, j);
        }
        for (int j = 0; j < 1000; j++) {
            delete array[j];
        }
    }
    auto end = std::chrono::system_clock::now();

    std::chrono::duration<double> diff = end - start;
    std::cout << "Time " << std::setw(9)
        << diff.count() << " s\n";
    return 0;


    /*int* ptr = new int(5);

    std::cout << "Ptr: " << ptr << " val: " << *ptr << std::endl;;

    delete ptr;*/

}