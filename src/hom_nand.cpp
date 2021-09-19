#include "hom_nand.hpp"

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

tlwe_lvl0 hom_nand(tlwe_lvl0& tlwe_a, tlwe_lvl0& tlwe_b, bootstrapping_key& bkey, key_switching_key& ks) {
    constexpr torus mu = 1 << (std::numeric_limits<torus>::digits - 3);
    constexpr size_t n = params::n;
    tlwe_lvl0 instance0;
    for(size_t i = 0; i < n; i++) {
        instance0.a[i] = 0 - tlwe_a.a[i] - tlwe_b.a[i];
    }
    instance0.b = mu - tlwe_a.b - tlwe_b.b;
    tlwe_lvl1 instance1 = gatebootstrapping_tlwe_to_tlwe(instance0, bkey);
    return identity_key_switching(instance1, ks);
}

}  // namespace TFHE