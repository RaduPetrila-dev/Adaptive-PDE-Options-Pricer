#include "Grid.hpp"
#include <algorithm>
#include <cmath>
#include <stdexcept>

// --- Grid base class ---

int Grid::size() const {
    return static_cast<int>(nodes_.size());
}

double Grid::spot(int i) const {
    return nodes_.at(i);
}

double Grid::spacing(int i) const {
    return nodes_.at(i + 1) - nodes_.at(i);
}

int Grid::findIndex(double S) const {
    if (S <= nodes_.front()) return 0;
    if (S >= nodes_.back()) return size() - 2;
    auto it = std::upper_bound(nodes_.begin(), nodes_.end(), S);
    return static_cast<int>(it - nodes_.begin()) - 1;
}

const std::vector<double>& Grid::nodes() const {
    return nodes_;
}

// --- UniformGrid ---

UniformGrid::UniformGrid(double S_max, int M) {
    if (M < 2 || S_max <= 0.0)
        throw std::invalid_argument("UniformGrid: need M >= 2, S_max > 0");
    nodes_.resize(M + 1);
    double dS = S_max / M;
    for (int i = 0; i <= M; ++i)
        nodes_[i] = i * dS;
}
