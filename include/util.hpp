#pragma once
#include <array>
#include <random>

#include "params.hpp"
using torus = uint32_t;

inline std::random_device rng;

inline torus double_to_torus(double d) {
    return static_cast<torus>(std::fmod(d, 1.0) * std::pow(2.0, std::numeric_limits<torus>::digits));
}

// uniform distribution in torus
inline torus torus_uniform_distribution() {
    std::uniform_int_distribution<torus> dist(0, std::numeric_limits<torus>::max());
    return dist(rng);
}

// modular nomal distribution D_T,alpha
inline torus modular_normal_distribution(double alpha) {
    std::normal_distribution<> dist(0.0, alpha);
    return double_to_torus(dist(rng));
}

// uniform distribution in binary
inline bool binary_uniform_distribution() {
    std::uniform_int_distribution<> dist(0, 1);
    return dist(rng);
}

// multiply in polynomial ring (naive)
// multiply: (T_N[X],T_N[X]) -> T_N[X]
template <typename T, typename T1, typename T2>
std::array<T, params::N> multiply(std::array<T1, params::N>& a, std::array<T2, params::N>& b) {
    size_t N = params::N;
    std::array<T, params::N> c;
    for(size_t i = 0; i < N; i++) {
        c[i] = 0;
    }
    for(size_t i = 0; i < N; i++) {
        for(size_t j = 0; j < N; j++) {
            if(i + j < N) {
                c[i + j] += a[i] * b[j];
            } else {
                c[i + j - N] -= a[i] * b[j];
            }
        }
    }
    return c;
}