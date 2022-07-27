#pragma once
#include <iostream>
#include "MemManager.h"


class Complex
{
public:
    Complex(double a, double b) : r(a), c(b)
    {
        aa = new(SM_NEW<unsigned char>()) unsigned char('q');
    }
    ~Complex()
    {
        if (r == -1)
        {
            std::cout << "Destructor called twice oof";
        }
        r = -1;

        SM_DELETE(aa);
    }
private:
    double r; // Real Part
    double c; // Complex Part
    unsigned char* aa;
};
