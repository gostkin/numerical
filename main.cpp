//
// Created by gosktin on 12.11.16.
//

#include <cstdlib>
#include <iostream>

#include "biginteger.h"

int main(int argc, char** argv) {
    BigInteger a(-868), b(-868);
    std::cout << (a += a) << ' ' << -868 - 868 << std::endl;

    return 0;
    for (int i = 0; i < 100000; ++i) {
        int a1 = (rand() % 1000) - 500, b1 = (rand() % 1000) - 500;
        if (a1 == 0)
            a1 = 0;
        if (b1 == 0)
            b1 = 0;

        BigInteger a(a1), b(b1);

        int old = a1;
        a += a;
        a1 += a1;
        if (BigInteger(a1) != a) {
            std::cout << old << ' ' << b1 << "FAIL" << std::endl;
            std::cout << a << ' ' << a1 << std::endl;
        }

    }
    std::cout << (BigInteger(-100) + BigInteger(1));

    return 0;
}