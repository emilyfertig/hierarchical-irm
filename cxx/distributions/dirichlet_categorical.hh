// Copyright 2024
// See LICENSE.txt

#pragma once
#include "base.hh"

class DirichletCategorical : public Distribution {
public:
    double alpha = 1;  // hyperparameter (applies to all categories)
    std::vector<int> counts;  // counts of observed categories
    PRNG *prng;

    DirichletCategorical(PRNG *prng, int k) {  // k is number of categories
        this->prng = prng;
        counts = std::vector<int>(k, 0);
    }
    void incorporate(double x) {
        assert(x >= 0 && x < counts.size());
        counts[size_t(x)] += 1;
    }
    void unincorporate(double x) {
        const size_t y = x;
        assert(y < counts.size());
        counts[y] -= 1;
        assert(0 <= counts[y]);
    }
    double logp(double x) const {
        assert(x >= 0 && x < counts.size());
        const int n = std::accumulate(counts.cbegin(), counts.cend(), 0);
        const double numer = log(alpha + counts[size_t(x)]);
        const double denom = log(n + alpha * counts.size());
        return numer - denom;
    }
    double logp_score() const {
        const size_t k = counts.size();
        const double a = alpha * k;
        const int n = std::accumulate(counts.cbegin(), counts.cend(), 0);
        const double lg = std::transform_reduce(
            counts.cbegin(), 
            counts.cend(), 
            0, 
            std::plus{},
            [&](size_t y) -> double {return lgamma(counts[y] + alpha); }
        );
        return lgamma(a) - lgamma(a + n) + lg - k * lgamma(alpha);
    }
    double sample() {
        vector<double> weights(counts.size());
        std::transform(
            counts.begin(), 
            counts.end(), 
            weights.begin(), 
            [&](size_t y) -> double { return y + alpha; }
        );
        int idx = choice(weights, prng);
        return double(idx);
    }

    // Disable copying.
    DirichletCategorical & operator=(const DirichletCategorical&) = delete;
    DirichletCategorical(const DirichletCategorical&) = delete;
};
