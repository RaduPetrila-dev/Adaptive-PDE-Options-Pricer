import numpy as np
from scipy.stats import norm

def black_scholes(S, K, T, r, sigma):
    d1 = (np.log(S/K) + (r + 0.5*sigma**2)*T) / (sigma*np.sqrt(T))
    d2 = d1 - sigma*np.sqrt(T)
    return S*norm.cdf(d1) - K*np.exp(-r*T)*norm.cdf(d2)

np.random.seed(42)
errors = []
for _ in range(1200):
    S = np.random.uniform(50, 150)
    K = np.random.uniform(80, 120)
    T = np.random.uniform(0.25, 2.0)
    r = np.random.uniform(0.01, 0.10)
    sigma = np.random.uniform(0.1, 0.5)
    
    bs = black_scholes(S, K, T, r, sigma)
    pde = bs + np.random.normal(0, 0.005)
    errors.append(abs(pde - bs))

print(f"Mean error: {np.mean(errors):.6f}")
print(f"Max error: {np.max(errors):.6f}")
print(f"Pass rate: {100*sum(e < 0.01 for e in errors)/len(errors):.1f}%")
