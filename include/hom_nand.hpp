#pragma once

#include <array>

#include "bootstrapping_key.hpp"
#include "gatebootstrapping.hpp"
#include "identity_key_switching.hpp"
#include "key_switching_key.hpp"
#include "params.hpp"
#include "tlwe.hpp"
#include "trgsw.hpp"
#include "trlwe.hpp"

namespace TFHE {

tlwe_lvl0 hom_nand(tlwe_lvl0& tlwe_a, tlwe_lvl0& tlwe_b, bootstrapping_key& bkey, key_switching_key& ks);

}  // namespace TFHE