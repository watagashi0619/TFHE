#include "key_switching_key.hpp"

#include <array>

#include "key.hpp"
#include "params.hpp"
#include "tlwe.hpp"

namespace TFHE {

std::array<std::array<tlwe_lvl0, ((1 << params::basebit) - 1)>, params::t> &key_switching_key::operator[](size_t i) {
    return ks[i];
}

const std::array<std::array<tlwe_lvl0, ((1 << params::basebit) - 1)>, params::t> &key_switching_key::operator[](size_t i) const {
    return ks[i];
}

key_switching_key::key_switching_key(secret_key &skey) {
    constexpr size_t N = params::N;
    constexpr size_t t = params::t;
    constexpr size_t basebit = params::basebit;
    for(size_t i = 0; i < N; i++) {
        for(size_t j = 0; j < t; j++) {
            for(size_t k = 1; k <= (1 << basebit) - 1; k++) {
                torus m = k * skey.lvl1[i] * (1 << (32 - (j + 1) * basebit));
                ks[i][j][k - 1] = tlwe_lvl0::encrypt_torus(skey, m);
            }
        }
    }
}

}  // namespace TFHE