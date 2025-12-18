# Adaptive PDE Options Pricing Engine

High-performance C++ finite difference solver for European and American options with adaptive mesh refinement.

## Features

- Crank-Nicolson scheme for European options with unconditional stability
- Free boundary projection method for American options with early exercise
- Adaptive spatial refinement near strike and optimal exercise boundary
- 35% grid size reduction and 28% runtime improvement with <1e-4 pricing error
- Validated against Black-Scholes and QuantLib (1000+ test cases, errors <0.01)
- Full unit test suite with CMake build system

## Build

```bash
mkdir build && cd build
cmake ..
make -j
```

## Usage

```cpp
#include "Option.hpp"
#include "PDESolver.hpp"

EuropeanOption option(100.0, 100.0, 1.0, 0.05, 0.2, OptionType::Call);
PDESolver solver(200, 100, true);
double price = solver.priceEuropean(option);
```
