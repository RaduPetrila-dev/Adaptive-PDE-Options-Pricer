# Adaptive PDE Options Pricing Engine

High-performance C++ finite difference solver for European and American options with adaptive mesh refinement.

[![Build and Test](https://github.com/RaduPetrila-dev/Adaptive-PDE-Options-Pricing-Engine/actions/workflows/ci.yml/badge.svg)](https://github.com/RaduPetrila-dev/Adaptive-PDE-Options-Pricing-Engine/actions/workflows/ci.yml)

## Features

- Crank-Nicolson scheme for European options with unconditional stability
- Free boundary projection method for American options with early exercise
- Adaptive spatial refinement near the strike price, reducing error by ~2.7x vs uniform grids at the same node count
- Non-uniform grid finite difference stencils with correct variable-spacing coefficients
- 50 unit tests covering European pricing, American constraints, grid properties, edge cases, and convergence
- Validated against Black-Scholes analytical prices across ATM/ITM/OTM, short/long maturity, and low/high volatility regimes
- Put-call parity verified numerically

## How the Adaptive Grid Works

The domain [0, S_max] is split into three regions:

| Region | Range | Spacing |
|--------|-------|---------|
| Lower | [0, K - 0.25K] | Coarse |
| Refinement | [K - 0.25K, K + 0.25K] | Fine (60% of nodes) |
| Upper | [K + 0.25K, S_max] | Coarse |

The option payoff has a kink at the strike K, where the solution gradient is steepest. Concentrating grid nodes here reduces interpolation error without increasing the total node count.

**Result (200 intervals, 200 time steps, ATM call):**
```
Adaptive error: 0.0014   (vs Black-Scholes)
Uniform  error: 0.0039   (vs Black-Scholes)
```

## Build

```bash
mkdir build && cd build
cmake ..
make -j
```

## Run

```bash
./pde_pricer
```

**Output:**
```
European Call (S=100, K=100, T=1, r=0.05, vol=0.20)
  Black-Scholes : 10.450584
  PDE (adaptive): 10.449163  error: 0.001421
  PDE (uniform) : 10.454438  error: 0.003854

American Put vs European Put (S=100, K=100)
  American PDE  : 6.086321
  European PDE  : 5.572105
  Early exercise: PASS (American >= European)
```

## Test

```bash
cd build && ctest --output-on-failure
```

50 tests across four suites:

- **European** (17 tests): ATM/ITM/OTM calls and puts, short/long maturity, high/low vol, varying rates, put-call parity, uniform convergence.
- **American** (8 tests): Early exercise premium (American >= European), intrinsic value floor, call equivalence without dividends, strict premium at high rates.
- **Grid** (10 tests): Boundary values, monotonicity, uniform spacing, adaptive refinement near strike, index lookup, invalid parameter rejection.
- **Edge cases** (15 tests): Input validation (negative spot, zero strike, negative vol), payoff correctness, non-negativity, grid convergence, adaptive vs uniform accuracy.

## Usage

```cpp
#include "Option.hpp"
#include "PDESolver.hpp"

// European call
Option call(100.0, 100.0, 1.0, 0.05, 0.2, OptionType::Call);
PDESolver solver(200, 200, true);  // 200 space, 200 time, adaptive grid
double price = solver.priceEuropean(call);

// American put
Option put(100.0, 100.0, 1.0, 0.05, 0.2, OptionType::Put, ExerciseType::American);
double am_price = solver.priceAmerican(put);
```

## Project Structure

```
├── include/
│   ├── Option.hpp          # Option parameters and payoff
│   ├── Grid.hpp            # UniformGrid and AdaptiveGrid
│   ├── PDESolver.hpp       # Crank-Nicolson solver
│   └── BlackScholes.hpp    # Analytical benchmark
├── src/
│   ├── Option.cpp
│   ├── Grid.cpp            # Grid base class + UniformGrid
│   ├── AdaptiveGrid.cpp    # Three-region adaptive grid
│   ├── PDESolver.cpp       # Non-uniform Crank-Nicolson
│   ├── BlackScholes.cpp
│   └── main.cpp
├── tests/
│   ├── CMakeLists.txt      # Google Test integration
│   ├── test_european.cpp
│   ├── test_american.cpp
│   ├── test_grid.cpp
│   └── test_edge_cases.cpp
├── validation/
│   └── validate_bs.py      # Python cross-validation script
├── CMakeLists.txt
└── .github/workflows/ci.yml
```

## Technical Notes

**Non-uniform grid FD stencils.** On a grid with variable spacing h+ and h-, the second derivative uses the standard three-point stencil:

```
d²V/dS² ≈ 2/(h+·h-·(h++h-)) · [h-·V_{i+1} - (h++h-)·V_i + h+·V_{i-1}]
```

This ensures second-order accuracy is maintained on the adaptive grid.

**American option pricing.** Uses the penalty/projection method: after each Crank-Nicolson time step, the solution is projected onto the payoff constraint V >= payoff(S). This enforces the early exercise boundary without explicitly tracking it.

## License

MIT
