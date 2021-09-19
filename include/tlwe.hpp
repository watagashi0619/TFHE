#pragma once

#include <array>
#include <random>

#include "key.hpp"
#include "util.hpp"

namespace TFHE {

template <int lvl>
struct tlwe {
    static constexpr size_t N();
    std::array<torus, N()> a;
    torus b;
    static std::array<bool, N()> key(secret_key& skey) noexcept;
    static tlwe<lvl> encrypt_torus(secret_key& skey, torus m);
    static tlwe<lvl> encrypt_binary(secret_key& skey, bool m);
    bool decrypt_binary(secret_key& skey);
};

template <int lvl>
constexpr size_t tlwe<lvl>::N() {
    if constexpr(lvl == 0) {
        return params::n;
    } else {
        return params::N;
    }
}

using tlwe_lvl0 = tlwe<0>;
using tlwe_lvl1 = tlwe<1>;

}  // namespace TFHE