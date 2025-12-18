#include "PDESolver.hpp"
#include <cmath>
#include <algorithm>

PDESolver::PDESolver(int n_space, int n_time, bool use_adaptive)
    : M(n_space), N(n_time), adaptive(use_adaptive) {
    alpha.resize(M + 1);
    beta.resize(M + 1);
    gamma.resize(M + 1);
}

void PDESolver::precomputeCoefficients(const Option& opt, double dS, double dt) {
    double sig2 = opt.sigma * opt.sigma;
    for (int i = 0; i <= M; ++i) {
        double S_i = i * dS;
        alpha[i] = 0.25 * dt * (sig2 * i * i - opt.r * i);
        beta[i] = -0.5 * dt * (sig2 * i * i + opt.r);
        gamma[i] = 0.25 * dt * (sig2 * i * i + opt.r * i);
    }
}

double PDESolver::priceEuropean(const Option& option) {
    double S_max = 3.0 * option.K;
    double dS = S_max / M;
    double dt = option.T / N;
    
    precomputeCoefficients(option, dS, dt);
    std::vector<double> V(M + 1);
    
    for (int i = 0; i <= M; ++i)
        V[i] = option.payoff(i * dS);
    
    for (int n = N - 1; n >= 0; --n)
        crankNicolson(V, option, dS, dt, S_max);
    
    int i = static_cast<int>(option.S / dS);
    if (i >= M) return V[M];
    double w = (option.S - i * dS) / dS;
    return (1 - w) * V[i] + w * V[i + 1];
}

double PDESolver::priceAmerican(const Option& option) {
    double S_max = 3.0 * option.K;
    double dS = S_max / M;
    double dt = option.T / N;
    
    precomputeCoefficients(option, dS, dt);
    std::vector<double> V(M + 1);
    
    for (int i = 0; i <= M; ++i)
        V[i] = option.payoff(i * dS);
    
    for (int n = N - 1; n >= 0; --n) {
        crankNicolson(V, option, dS, dt, S_max);
        freeProjection(V, option, dS);
    }
    
    int i = static_cast<int>(option.S / dS);
    if (i >= M) return V[M];
    double w = (option.S - i * dS) / dS;
    return (1 - w) * V[i] + w * V[i + 1];
}

void PDESolver::crankNicolson(std::vector<double>& V, const Option& opt, 
                              double dS, double dt, double S_max) {
    std::vector<double> a(M + 1), b(M + 1), c(M + 1), d(M + 1);
    
    for (int i = 1; i < M; ++i) {
        a[i] = -alpha[i];
        b[i] = 1.0 - beta[i];
        c[i] = -gamma[i];
        d[i] = alpha[i] * V[i-1] + (1.0 + beta[i]) * V[i] + gamma[i] * V[i+1];
    }
    
    b[0] = 1.0; c[0] = 0.0;
    d[0] = (opt.type == OptionType::Call) ? 0.0 : opt.K * std::exp(-opt.r * dt);
    
    a[M] = 0.0; b[M] = 1.0;
    d[M] = (opt.type == OptionType::Call) ? 
           S_max - opt.K * std::exp(-opt.r * dt) : 0.0;
    
    solveTridiagonal(a, b, c, d, V);
}

void PDESolver::freeProjection(std::vector<double>& V, const Option& opt, double dS) {
    for (int i = 0; i <= M; ++i)
        V[i] = std::max(V[i], opt.payoff(i * dS));
}

void PDESolver::solveTridiagonal(const std::vector<double>& a, 
                                 const std::vector<double>& b,
                                 const std::vector<double>& c, 
                                 const std::vector<double>& d,
                                 std::vector<double>& x) {
    int n = d.size();
    std::vector<double> cp(n), dp(n);
    
    cp[0] = c[0] / b[0];
    dp[0] = d[0] / b[0];
    
    for (int i = 1; i < n; ++i) {
        double m = 1.0 / (b[i] - a[i] * cp[i-1]);
        cp[i] = c[i] * m;
        dp[i] = (d[i] - a[i] * dp[i-1]) * m;
    }
    
    x[n-1] = dp[n-1];
    for (int i = n - 2; i >= 0; --i)
        x[i] = dp[i] - cp[i] * x[i+1];
}
