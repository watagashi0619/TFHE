#include "identity_key_switching.hpp"

#include <array>

#include "key.hpp"
#include "key_switching_key.hpp"
#include "params.hpp"
#include "tlwe.hpp"

using namespace TFHE;
/*
tlwe_lvl0 identity_key_switching(tlwe_lvl1 tlwe_lvl1, key_switching_key &ks) {

    constexpr size_t n = params::n;
    constexpr size_t N = params::N;
    constexpr size_t t = params::t;
    constexpr size_t basebit = params::basebit;

    tlwe_lvl0 tlwe_lvl0_out;
    // initialize
    for(size_t i = 0; i < n; i++) {
        tlwe_lvl0_out.a[i] = 0;
    }
    tlwe_lvl0_out.b = tlwe_lvl1.b;
    torus prec_offset = 1 << (32 - (1 + basebit * t));
    for(size_t i = 0; i < N; i++) {
        torus a_bar = tlwe_lvl1.a[i] + prec_offset;
        for(size_t j = 0; j < t; j++) {
            torus k = (a_bar >> (32 - (j + 1) * basebit)) & ((1 << basebit) - 1);
            if(k == 0) {
                continue;
            }
            // (.a,.b)-=KS_ijk
            for(size_t l = 0; l < n; l++) {
                tlwe_lvl0_out.a[l] -= ks[i][j][k - 1].a[l];
            }
            tlwe_lvl0_out.b -= ks[i][j][k - 1].b;
        }
    }

    return tlwe_lvl0_out;

}
*/