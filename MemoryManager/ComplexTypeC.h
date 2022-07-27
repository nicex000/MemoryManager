#pragma once
#include <iostream>

class Complex
{
public:
    Complex(double a, double b) : r(a), c(b)
    {
        aa = new unsigned char('a');
    }
    ~Complex()
    {
        if (r == -1)
        {
            std::cout << "Destructor called twice oof";
        }
        r = -1;
        delete aa;
    }
private:
    double r;
    double c; 
    unsigned char* aa;
};
