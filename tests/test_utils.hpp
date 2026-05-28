#pragma once

#include <cmath>
#include <iostream>
#include <string>

inline void assert_true(bool condition, const std::string &message)
{
    if (!condition)
    {
        std::cerr << "FAIL: " << message << "\n";
        std::exit(EXIT_FAILURE);
    }
}

inline void assert_eq(int actual, int expected, const std::string &message)
{
    if (actual != expected)
    {
        std::cerr << "FAIL: " << message << " (expected=" << expected << ", actual=" << actual << ")\n";
        std::exit(EXIT_FAILURE);
    }
}

inline void assert_eq(float actual, float expected, const std::string &message, float epsilon = 1e-4f)
{
    if (std::fabs(actual - expected) > epsilon)
    {
        std::cerr << "FAIL: " << message << " (expected=" << expected << ", actual=" << actual << ")\n";
        std::exit(EXIT_FAILURE);
    }
}

inline void assert_near(float actual, float expected, float epsilon, const std::string &message)
{
    if (std::fabs(actual - expected) > epsilon)
    {
        std::cerr << "FAIL: " << message << " (expected=" << expected << ", actual=" << actual << ", eps=" << epsilon << ")\n";
        std::exit(EXIT_FAILURE);
    }
}
