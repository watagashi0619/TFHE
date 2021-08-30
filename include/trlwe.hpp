#pragma once
#include <array>

#include "key.hpp"
#include "params.hpp"
#include "tlwe.hpp"
#include "util.hpp"

// TRLWE
// (a,b) in (T_N[X],T_N[X])

struct trlwe {
    trlwe();
    std::array<torus, params::N> a, b;

    static trlwe encrypt_polynomial_torus(secret_key skey, std::array<torus, params::N> m);
    static trlwe encrypt_polynomial_zero(secret_key skey);
    static trlwe encrypt_polynomial_binary(secret_key skey, std::array<bool, params::N> m);
    std::array<bool, params::N> decrypt_polynomial_binary(secret_key skey);

    static tlwe_lvl1 sample_extract_index(trlwe trlwe, size_t k);
    static std::array<std::array<int, params::N>, params::l> decomposition(std::array<torus, params::N> a);
};