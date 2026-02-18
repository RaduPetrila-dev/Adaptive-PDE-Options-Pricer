#include "PDESolver.hpp"
#include <cmath>
#include <algorithm>
#include <stdexcept>

PDESolver::PDESolver(int n_space, int n_time, bool use_adaptive)
    : M_(n_space), N_(n_time), adaptive_(use_adaptive) {
    if (M_ < 10 || N_ < 1)
        throw std::invalid_argument("PDESolver: need n_space >= 10, n_time >= 1");
}

int PDESolver::gridSize() const {
    return grid_ ? grid_->size() : 0;
}

// ----------------------------------------------------------------
// Grid construction
// ----------------------------------------------------------------

void PDESolver::buildGrid(const Option& opt) {
    double S_max = 3.0 * opt.K;
    if (adaptive_)
        grid_ = std::make_unique<AdaptiveGrid>(S_max, M_, opt.K);
    else
        grid_ = std::make_unique<UniformGrid>(S_max, M_);
}

// ----------------------------------------------------------------
// Spatial operator coefficients for the Black-Scholes PDE
//
//   dV/dt + 0.5*sig^2*S^2 * d2V/dS2 + r*S * dV/dS - r*V = 0
//
// On a non-uniform grid with h+ = S_{i+1}-S_i, h- = S_i-S_{i-1}:
//
//   d2V/dS2 ≈ 2/(h+*h-*(h++h-)) * [h-*V_{i+1} - (h++h-)*V_i + h+*V_{i-1}]
//   dV/dS   ≈ 1/(h+*h-*(h++h-)) * [h-^2*V_{i+1} + (h+^2-h-^2)*V_i - h+^2*V_{i-1}]
// ----------------------------------------------------------------

std::vector<PDESolver::Coefficients>
PDESolver::computeCoefficients(const Option& opt) const {
    int n = grid_->size();
    std::vector<Coefficients> coeff(n, {0.0, 0.0, 0.0});
    double sig2 = opt.sigma * opt.sigma;

    for (int i = 1; i < n - 1; ++i) {
        double Si = grid_->spot(i);
        double hp = grid_->spacing(i);      // S_{i+1} - S_i
        double hm = grid_->spacing(i - 1);  // S_i - S_{i-1}
        double hsum = hp + hm;
        double denom = hp * hm * hsum;

        // Second derivative stencil weights
        double d2_lo =  2.0 * hp / denom;
        double d2_mid = -2.0 * hsum / denom;
        double d2_hi =  2.0 * hm / denom;

        // First derivative stencil weights
        double d1_lo = -(hp * hp) / denom;
        double d1_mid = (hp * hp - hm * hm) / denom;
        double d1_hi =  (hm * hm) / denom;

        double half_sig2_S2 = 0.5 * sig2 * Si * Si;
        double rS = opt.r * Si;

        coeff[i].a = half_sig2_S2 * d2_lo  + rS * d1_lo;
        coeff[i].b = half_sig2_S2 * d2_mid + rS * d1_mid - opt.r;
        coeff[i].c = half_sig2_S2 * d2_hi  + rS * d1_hi;
    }
    return coeff;
}

// ----------------------------------------------------------------
// One Crank-Nicolson time step (implicit average of n and n+1).
//
//   LHS_i * V^n = RHS_i * V^{n+1}
//
//   LHS:  -0.5*dt*a_i * V_{i-1} + (1 - 0.5*dt*b_i) * V_i - 0.5*dt*c_i * V_{i+1}
//   RHS:   0.5*dt*a_i * V_{i-1} + (1 + 0.5*dt*b_i) * V_i + 0.5*dt*c_i * V_{i+1}
// ----------------------------------------------------------------

void PDESolver::crankNicolsonStep(std::vector<double>& V,
                                  const std::vector<Coefficients>& coeff,
                                  double dt) const {
    int n = grid_->size();
    std::vector<double> lower(n, 0.0), diag(n, 0.0), upper(n, 0.0), rhs(n, 0.0);

    // Boundary: i = 0
    diag[0] = 1.0;
    rhs[0] = V[0];

    // Interior nodes
    for (int i = 1; i < n - 1; ++i) {
        double ha = 0.5 * dt * coeff[i].a;
        double hb = 0.5 * dt * coeff[i].b;
        double hc = 0.5 * dt * coeff[i].c;

        // LHS (tridiagonal matrix)
        lower[i] = -ha;
        diag[i]  = 1.0 - hb;
        upper[i] = -hc;

        // RHS (explicit side)
        rhs[i] = ha * V[i - 1] + (1.0 + hb) * V[i] + hc * V[i + 1];
    }

    // Boundary: i = n-1
    diag[n - 1] = 1.0;
    rhs[n - 1] = V[n - 1];

    solveTridiagonal(lower, diag, upper, rhs, V);
}

// ----------------------------------------------------------------
// American early exercise: V_i = max(V_i, payoff(S_i))
// ----------------------------------------------------------------

void PDESolver::applyEarlyExercise(std::vector<double>& V,
                                   const Option& opt) const {
    int n = grid_->size();
    for (int i = 0; i < n; ++i)
        V[i] = std::max(V[i], opt.payoff(grid_->spot(i)));
}

// ----------------------------------------------------------------
// Linear interpolation to find price at the exact spot S.
// ----------------------------------------------------------------

double PDESolver::interpolate(const std::vector<double>& V, double S) const {
    int i = grid_->findIndex(S);
    double S_lo = grid_->spot(i);
    double S_hi = grid_->spot(i + 1);
    double w = (S - S_lo) / (S_hi - S_lo);
    return (1.0 - w) * V[i] + w * V[i + 1];
}

// ----------------------------------------------------------------
// Public pricing functions
// ----------------------------------------------------------------

double PDESolver::priceEuropean(const Option& option) {
    buildGrid(option);
    int n = grid_->size();
    double dt = option.T / N_;

    auto coeff = computeCoefficients(option);

    // Terminal condition: V(S, T) = payoff(S)
    std::vector<double> V(n);
    for (int i = 0; i < n; ++i)
        V[i] = option.payoff(grid_->spot(i));

    // Boundary conditions at S = 0 and S = S_max for each time step.
    double S_max = grid_->spot(n - 1);
    for (int step = N_ - 1; step >= 0; --step) {
        double tau = (N_ - step) * dt;  // time remaining
        if (option.type == OptionType::Call) {
            V[0] = 0.0;
            V[n - 1] = S_max - option.K * std::exp(-option.r * tau);
        } else {
            V[0] = option.K * std::exp(-option.r * tau);
            V[n - 1] = 0.0;
        }
        crankNicolsonStep(V, coeff, dt);
    }

    return interpolate(V, option.S);
}

double PDESolver::priceAmerican(const Option& option) {
    buildGrid(option);
    int n = grid_->size();
    double dt = option.T / N_;

    auto coeff = computeCoefficients(option);

    std::vector<double> V(n);
    for (int i = 0; i < n; ++i)
        V[i] = option.payoff(grid_->spot(i));

    double S_max = grid_->spot(n - 1);
    for (int step = N_ - 1; step >= 0; --step) {
        double tau = (N_ - step) * dt;
        if (option.type == OptionType::Call) {
            V[0] = 0.0;
            V[n - 1] = S_max - option.K * std::exp(-option.r * tau);
        } else {
            V[0] = option.K * std::exp(-option.r * tau);
            V[n - 1] = 0.0;
        }
        crankNicolsonStep(V, coeff, dt);
        applyEarlyExercise(V, option);
    }

    return interpolate(V, option.S);
}

// ----------------------------------------------------------------
// Thomas algorithm for tridiagonal systems.
// ----------------------------------------------------------------

void PDESolver::solveTridiagonal(const std::vector<double>& a,
                                 const std::vector<double>& b,
                                 const std::vector<double>& c,
                                 const std::vector<double>& d,
                                 std::vector<double>& x) {
    int n = static_cast<int>(d.size());
    std::vector<double> cp(n), dp(n);

    cp[0] = c[0] / b[0];
    dp[0] = d[0] / b[0];

    for (int i = 1; i < n; ++i) {
        double m = 1.0 / (b[i] - a[i] * cp[i - 1]);
        cp[i] = c[i] * m;
        dp[i] = (d[i] - a[i] * dp[i - 1]) * m;
    }

    x[n - 1] = dp[n - 1];
    for (int i = n - 2; i >= 0; --i)
        x[i] = dp[i] - cp[i] * x[i + 1];
}
