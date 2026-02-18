#pragma once
#include "Option.hpp"
#include "Grid.hpp"
#include <vector>
#include <memory>

class PDESolver {
public:
    // n_space = number of spatial intervals, n_time = number of time steps.
    // use_adaptive = true builds an AdaptiveGrid; false builds a UniformGrid.
    PDESolver(int n_space, int n_time, bool use_adaptive = true);

    double priceEuropean(const Option& option);
    double priceAmerican(const Option& option);

    // Expose grid size for diagnostics (adaptive grid may differ from n_space).
    int gridSize() const;

private:
    int M_, N_;
    bool adaptive_;

    // Per-node spatial operator coefficients: L*V_i = a_i*V_{i-1} + b_i*V_i + c_i*V_{i+1}
    struct Coefficients { double a, b, c; };

    std::unique_ptr<Grid> grid_;

    void buildGrid(const Option& opt);
    std::vector<Coefficients> computeCoefficients(const Option& opt) const;
    void crankNicolsonStep(std::vector<double>& V,
                           const std::vector<Coefficients>& coeff,
                           double dt) const;
    void applyEarlyExercise(std::vector<double>& V, const Option& opt) const;
    double interpolate(const std::vector<double>& V, double S) const;

    static void solveTridiagonal(const std::vector<double>& lower,
                                 const std::vector<double>& diag,
                                 const std::vector<double>& upper,
                                 const std::vector<double>& rhs,
                                 std::vector<double>& x);
};
