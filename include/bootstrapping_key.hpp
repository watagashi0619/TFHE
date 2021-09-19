#pragma once

#include <array>
#include <random>
#include <vector>

#include "key.hpp"
#include "params.hpp"
#include "trgsw.hpp"
namespace TFHE {

// bootstrapping key
// TRGSW^n
// TRGSW[i] = (encrypt secret_key_lvl0[i] to trgsw)
struct bootstrapping_key {
    std::array<trgsw, params::n> data;
    bootstrapping_key(secret_key &skey);

    trgsw &operator[](size_t i);
    const trgsw &operator[](size_t i) const;
};

}  // namespace TFHE