#pragma once
#include <array>

#include "key.hpp"
#include "params.hpp"
#include "trlwe.hpp"

// TRGSW
// trlwe^(2l) = [(a_0[X],b_0[X]),...,(a_{2l-1}[X],b_{2l-1}[X])]
namespace TFHE {

struct trgsw {
    std::array<trlwe, 2 * params::l> secret_message;

    trlwe& operator[](size_t i);

    const trlwe& operator[](size_t i) const;

    static trgsw encrypt_polynomial_int(secret_key& skey, std::array<int, params::N> mu);
    static trgsw encrypt_binary(secret_key& skey, bool mu);
};

trlwe external_product(trgsw& trgsw, trlwe& trlwe_in);
trlwe cmux(trgsw& trgsw, trlwe& trlwe0, trlwe& trlwe1);

}  // namespace TFHE