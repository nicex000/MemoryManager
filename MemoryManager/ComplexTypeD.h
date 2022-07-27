#pragma once
#include <iostream>

class Complex
{
public:
    Complex(double a, double b) : r(a), c(b)
    {
        bigNumberA = bigNumberB = bigNumberC = bigNumberD = bigNumberE = bigNumberF = bigNumberG = bigNumberH = 1;
    }
    ~Complex()
    {
        if (r == -1)
        {
            std::cout << "Destructor called twice oof";
        }
        r = -1;
    }
private:
    double r;
    double c;
    int64_t bigNumberA, bigNumberB, bigNumberC, bigNumberD, bigNumberE, bigNumberF, bigNumberG, bigNumberH;
};
