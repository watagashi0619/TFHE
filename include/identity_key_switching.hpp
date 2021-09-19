#pragma once

#include <array>

#include "key_switching_key.hpp"
#include "params.hpp"
#include "tlwe.hpp"

namespace TFHE {

tlwe_lvl0 identity_key_switching(tlwe_lvl1& tlwe_lvl1, key_switching_key& ks);

}  // namespace TFHE