#pragma once

#include <array>

#include "key.hpp"
#include "params.hpp"
#include "trgsw.hpp"
#include "trlwe.hpp"

// TBSR
namespace TFHE {

struct tbsr {
    std::array<trlwe, params::tbsr_width> data;
    tbsr(secret_key& skey);

    trlwe& operator[](size_t i);
    const trlwe& operator[](size_t i) const;

    static std::array<trgsw, params::tbsr_width> gen_trgsw_set(secret_key& skey, int x);

    static tbsr add(secret_key& skey, std::array<trgsw, params::tbsr_width>& trgsw_set_a, std::array<trgsw, params::tbsr_width>& trgsw_set_b);

    int decrypt_tbsr(secret_key& skey);
};

}  // namespace TFHE
