#include <iostream>
#include <iomanip>
#include "Option.hpp"
#include "PDESolver.hpp"
#include "BlackScholes.hpp"

int main() {
    std::cout << "Adaptive PDE Options Pricer\n";
    std::cout << "============================\n\n";
    
    Option call(100.0, 100.0, 1.0, 0.05, 0.2, OptionType::Call);
    PDESolver solver(200, 100, true);
    
    double pde_price = solver.priceEuropean(call);
    double bs_price = BlackScholes::price(call);
    
    std::cout << std::fixed << std::setprecision(6);
    std::cout << "European Call:\n";
    std::cout << "  PDE: " << pde_price << "\n";
    std::cout << "  BS:  " << bs_price << "\n";
    std::cout << "  Err: " << std::abs(pde_price - bs_price) << "\n";
    
    return 0;
}
