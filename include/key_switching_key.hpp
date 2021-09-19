#pragma once

#include <array>

#include "key.hpp"
#include "params.hpp"
#include "tlwe.hpp"

namespace TFHE {

struct key_switching_key {
    // note: require a lot of memory
    std::array<std::array<std::array<tlwe_lvl0, ((1 << params::basebit) - 1)>, params::t>, params::N> ks;

    std::array<std::array<tlwe_lvl0, ((1 << params::basebit) - 1)>, params::t> &operator[](size_t i);

    const std::array<std::array<tlwe_lvl0, ((1 << params::basebit) - 1)>, params::t> &operator[](size_t i) const;

    key_switching_key(secret_key &skey);
};

}  // namespace TFHE