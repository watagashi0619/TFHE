#pragma once

#include <array>
#include <random>

#include "bootstrapping_key.hpp"
#include "key.hpp"
#include "params.hpp"
#include "trgsw.hpp"

namespace TFHE {

template <typename T, size_t N>
std::array<T, N> poly_mult_by_X_k(std::array<T, N> poly_in, size_t k);

trlwe test_vector();
trlwe blind_rotate(tlwe_lvl0 tlwe, bootstrapping_key& bkey, trlwe trlwe);
tlwe_lvl1 gatebootstrapping_tlwe_to_tlwe(tlwe_lvl0 tlwe, bootstrapping_key& bkey);

}  // namespace TFHE