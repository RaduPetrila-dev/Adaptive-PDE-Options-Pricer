#pragma once
#include "Option.hpp"

class BlackScholes {
public:
    static double price(const Option& option);
    static double delta(const Option& option);
private:
    static double normalCDF(double x);
};
