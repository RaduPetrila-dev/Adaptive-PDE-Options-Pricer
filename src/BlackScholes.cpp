#include "BlackScholes.hpp"
#include <cmath>

double BlackScholes::normalCDF(double x) {
    return 0.5 * std::erfc(-x * M_SQRT1_2);
}

double BlackScholes::price(const Option& opt) {
    double d1 = (std::log(opt.S / opt.K) + 
                (opt.r + 0.5 * opt.sigma * opt.sigma) * opt.T) /
                (opt.sigma * std::sqrt(opt.T));
    double d2 = d1 - opt.sigma * std::sqrt(opt.T);
    
    if (opt.type == OptionType::Call)
        return opt.S * normalCDF(d1) - 
               opt.K * std::exp(-opt.r * opt.T) * normalCDF(d2);
    return opt.K * std::exp(-opt.r * opt.T) * normalCDF(-d2) - 
           opt.S * normalCDF(-d1);
}

double BlackScholes::delta(const Option& opt) {
    double d1 = (std::log(opt.S / opt.K) + 
                (opt.r + 0.5 * opt.sigma * opt.sigma) * opt.T) /
                (opt.sigma * std::sqrt(opt.T));
    if (opt.type == OptionType::Call)
        return normalCDF(d1);
    return normalCDF(d1) - 1.0;
}
