
#include "tlwe.hpp"

#include <array>

#include "key.hpp"
#include "params.hpp"
#include "util.hpp"

namespace TFHE {

template <int lvl>
std::array<bool, tlwe<lvl>::N()> tlwe<lvl>::key(secret_key& skey) noexcept {
    if constexpr(lvl == 0) {
        return skey.lvl0;
    } else {
        return skey.lvl1;
    }
}

template <int lvl>
tlwe<lvl> tlwe<lvl>::encrypt_torus(secret_key& skey, torus m) {
    tlwe<lvl> instance;
    auto s = key(skey);
    for(size_t i = 0; i < instance.N(); i++) {
        instance.a[i] = torus_uniform_distribution();
    }
    torus e = modular_normal_distribution(params::alpha);
    instance.b = m + e;
    for(size_t i = 0; i < instance.N(); i++) {
        instance.b += instance.a[i] * s[i];
    }
    return instance;
}

template <int lvl>
tlwe<lvl> tlwe<lvl>::encrypt_binary(secret_key& skey, bool m) {
    const torus mu = 1 << (std::numeric_limits<torus>::digits - 3);
    if(m) {
        return encrypt_torus(skey, mu);
    } else {
        return encrypt_torus(skey, -mu);
    }
}

template <int lvl>
bool tlwe<lvl>::decrypt_binary(secret_key& skey) {
    int32_t m = b;
    auto s = key(skey);
    for(size_t i = 0; i < N(); i++) {
        m -= a[i] * s[i];
    }
    return m > 0;
}

template tlwe<0> tlwe<0>::encrypt_binary(secret_key& skey, bool m);
template tlwe<1> tlwe<1>::encrypt_binary(secret_key& skey, bool m);
template bool tlwe<0>::decrypt_binary(secret_key& skey);
template bool tlwe<1>::decrypt_binary(secret_key& skey);

}  // namespace TFHE