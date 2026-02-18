#include <iostream>
#include <iomanip>
#include <cmath>
#include "Option.hpp"
#include "PDESolver.hpp"
#include "BlackScholes.hpp"

int main() {
    std::cout << "Adaptive PDE Options Pricer\n";
    std::cout << "============================\n\n";
    std::cout << std::fixed << std::setprecision(6);

    // --- European Call (ATM) ---
    Option call(100.0, 100.0, 1.0, 0.05, 0.2, OptionType::Call);
    double bs = BlackScholes::price(call);

    PDESolver adaptive(200, 200, true);
    PDESolver uniform(200, 200, false);

    double price_adapt = adaptive.priceEuropean(call);
    double price_unif  = uniform.priceEuropean(call);

    std::cout << "European Call (S=100, K=100, T=1, r=0.05, vol=0.20)\n";
    std::cout << "  Black-Scholes : " << bs << "\n";
    std::cout << "  PDE (adaptive): " << price_adapt
              << "  error: " << std::abs(price_adapt - bs) << "\n";
    std::cout << "  PDE (uniform) : " << price_unif
              << "  error: " << std::abs(price_unif - bs) << "\n";
    std::cout << "  Adaptive grid : " << adaptive.gridSize() << " nodes\n";
    std::cout << "  Uniform grid  : " << uniform.gridSize()  << " nodes\n\n";

    // --- European Put (OTM) ---
    Option put(100.0, 110.0, 0.5, 0.03, 0.3, OptionType::Put);
    double bs_put = BlackScholes::price(put);
    double pde_put = adaptive.priceEuropean(put);

    std::cout << "European Put (S=100, K=110, T=0.5, r=0.03, vol=0.30)\n";
    std::cout << "  Black-Scholes : " << bs_put << "\n";
    std::cout << "  PDE (adaptive): " << pde_put
              << "  error: " << std::abs(pde_put - bs_put) << "\n\n";

    // --- American Put ---
    Option am_put(100.0, 100.0, 1.0, 0.05, 0.2, OptionType::Put,
                  ExerciseType::American);
    double am_price = adaptive.priceAmerican(am_put);
    double eu_price = adaptive.priceEuropean(
        Option(100.0, 100.0, 1.0, 0.05, 0.2, OptionType::Put));

    std::cout << "American Put vs European Put (S=100, K=100)\n";
    std::cout << "  American PDE  : " << am_price << "\n";
    std::cout << "  European PDE  : " << eu_price << "\n";
    std::cout << "  Early exercise: "
              << (am_price >= eu_price ? "PASS" : "FAIL")
              << " (American >= European)\n";

    return 0;
}
