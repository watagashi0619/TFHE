#include "trgsw.hpp"

#include <array>
#include <random>

#include "key.hpp"
#include "params.hpp"
#include "tlwe.hpp"
#include "trlwe.hpp"
#include "util.hpp"

// TRGSW
// trlwe^(2l) = [(a_0[X],b_0[X]),...,(a_{2l-1}[X],b_{2l-1}[X])]
namespace TFHE {

trgsw::trgsw() {}

trlwe& trgsw::operator[](size_t i) {
    return secret_message[i];
}

const trlwe& trgsw::operator[](size_t i) const {
    return secret_message[i];
}

// encryot_polynomial_int: mu in Z_N[X] -> trgsw
trgsw trgsw::encrypt_polynomial_int(secret_key skey, std::array<int, params::N> mu) {
    constexpr size_t N = params::N;
    constexpr size_t l = params::l;
    constexpr size_t Bgbit = params::Bgbit;

    trgsw instance = trgsw();

    for(size_t i = 0; i < 2 * l; i++) {
        instance[i] = trlwe::encrypt_polynomial_zero(skey);
    }

    // trgsw[i].a += mu[X]/Bg^(i+1)
    // trgsw[i+l].b += mu[X]/Bg^(i+1) 0<=i<=l-1
    // mu[X]/Bg^(i+1)=x mod 2^32-1
    // mu[X]=x Bg^(i+1) mod 2^32-1
    // mu[X] 2^32 Bg^(-i-1)=x mod 2^32-1
    for(size_t i = 0; i < l; i++) {
        for(size_t j = 0; j < N; j++) {
            torus t = static_cast<torus>(mu[j]) * (1u << (32 - Bgbit * (i + 1)));
            instance[i].a[j] += t;
            instance[i + l].b[j] += t;
        }
    }

    // trgsw = (
    //   (mu[X]/Bg+a_0[X]      ,b_0[X]                  )
    //   ...
    //   (mu[X]/Bg^l+a_{l-1}   ,b_{l-1}[X]              )
    //   (a_l[X]               ,mu[X]/Bg+b_l[X]         )
    //   ...
    //   (a_{2l-1}             ,mu[X]/Bg^l+b_{2l-1}[X]  )
    //  )
    return instance;
}

// encrypt_binary: mu in B -> trgsw
trgsw trgsw::encrypt_binary(secret_key skey, bool mu) {
    std::array<int, params::N> t = {};
    t[0] = mu ? 1 : 0;
    return encrypt_polynomial_int(skey, t);
}

// external product: (trgsw,trlwe) -> trlwe
trlwe external_product(trgsw trgsw, trlwe trlwe_in) {
    constexpr size_t N = params::N;
    constexpr size_t l = params::l;
    std::array<std::array<int, N>, l> decomposition_a = decomposition(trlwe_in.a);
    std::array<std::array<int, N>, l> decomposition_b = decomposition(trlwe_in.b);
    trlwe trlwe_out;

    for(size_t i = 0; i < params::N; i++) {
        trlwe_out.a[i] = 0;
        trlwe_out.b[i] = 0;
    }

    std::array<int, N> tmp;

    // trgsw_out=(decomposition_a,decomposition_b)(trgsw_in=(TRLWE)^2l)
    // (decomposition_a,decomposition_b)=(a_bar_0[X],...,a_bar_{l-1}[X],b_bar_0[X],...,b_bar_{l-1}[X])
    // trgsw_in=(
    //   (mu[X]/Bg+a_0[X]      ,b_0[X]                  )
    //   ...
    //   (mu[X]/Bg^l+a_{l-1}   ,b_{l-1}[X]              )
    //   (a_l[X]               ,mu[X]/Bg+b_l[X]         )
    //   ...
    //   (a_{2l-1}             ,mu[X]/Bg^l+b_{2l-1}[X]  )
    //  )
    for(size_t i = 0; i < l; i++) {
        tmp = multiply<int>(decomposition_a[i], trgsw[i].a);
        for(size_t j = 0; j < N; j++) {
            trlwe_out.a[j] += tmp[j];
        }
        tmp = multiply<int>(decomposition_b[i], trgsw[i + l].a);
        for(size_t j = 0; j < N; j++) {
            trlwe_out.a[j] += tmp[j];
        }
        tmp = multiply<int>(decomposition_a[i], trgsw[i].b);
        for(size_t j = 0; j < N; j++) {
            trlwe_out.b[j] += tmp[j];
        }
        tmp = multiply<int>(decomposition_b[i], trgsw[i + l].b);
        for(size_t j = 0; j < N; j++) {
            trlwe_out.b[j] += tmp[j];
        }
    }
    return trlwe_out;
}

// cmux
trlwe cmux(trgsw trgsw, trlwe trlwe0, trlwe trlwe1) {
    constexpr size_t N = params::N;
    trlwe trlwe_out;
    for(size_t j = 0; j < N; j++) {
        trlwe_out.a[j] = trlwe0.a[j] - trlwe1.a[j];
        trlwe_out.b[j] = trlwe0.b[j] - trlwe1.b[j];
    }
    trlwe_out = external_product(trgsw, trlwe_out);
    for(size_t j = 0; j < N; j++) {
        trlwe_out.a[j] += trlwe1.a[j];
        trlwe_out.b[j] += trlwe1.b[j];
    }
    return trlwe_out;
}

}  // namespace TFHE