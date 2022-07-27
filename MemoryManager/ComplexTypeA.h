#pragma once
#include <iostream>

class Complex
{
public:
    Complex(double a, double b) : r(a), c(b) {}
    ~Complex()
    {
        if (r == -1)
        {
            std::cout << "Destructor called twice oof";
        }
        r = -1;
    }
private:
    double r; // Real Part
    double c; // Complex Part
};
