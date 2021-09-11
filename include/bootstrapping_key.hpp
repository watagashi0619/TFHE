#pragma once

#include <array>
#include <random>
#include <vector>

#include "key.hpp"
#include "params.hpp"
#include "trgsw.hpp"
namespace TFHE {

struct bootstrapping_key {
    std::array<trgsw, params::n> data;
    bootstrapping_key(secret_key skey);

    trgsw &operator[](size_t i);
    const trgsw &operator[](size_t i) const;
};

}  // namespace TFHE