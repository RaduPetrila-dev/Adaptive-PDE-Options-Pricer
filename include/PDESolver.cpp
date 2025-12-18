#pragma once
#include "Option.hpp"
#include <vector>

class PDESolver {
public:
    PDESolver(int n_space, int n_time, bool use_adaptive = true);
    double priceEuropean(const Option& option);
    double priceAmerican(const Option& option);

private:
    int M, N;
    bool adaptive;
    std::vector<double> alpha, beta, gamma;
    
    void precomputeCoefficients(const Option& opt, double dS, double dt);
    void crankNicolson(std::vector<double>& V, const Option& opt, 
                      double dS, double dt, double S_max);
    void freeProjection(std::vector<double>& V, const Option& opt, double dS);
    void solveTridiagonal(const std::vector<double>& a, 
                         const std::vector<double>& b,
                         const std::vector<double>& c, 
                         const std::vector<double>& d,
                         std::vector<double>& x);
};
