#include "trlwe.hpp"

#include <array>

#include "key.hpp"
#include "params.hpp"
#include "tlwe.hpp"
#include "util.hpp"

trlwe::trlwe() {}

// encrypt_polynomial_torus: message in T_N[X] -> trlwe
trlwe trlwe::encrypt_polynomial_torus(secret_key skey, std::array<torus, params::N> m) {
    constexpr size_t N = params::N;
    trlwe instance = trlwe();
    for(size_t i = 0; i < N; i++) {
        instance.a[i] = torus_uniform_distribution();
    }
    std::array<torus, N> e;
    for(size_t i = 0; i < N; i++) {
        e[i] = modular_normal_distribution(params::alpha_bk);
    }
    std::array<torus, N> s;
    for(size_t i = 0; i < N; i++) {
        s[i] = skey.lvl1[i];
    }
    instance.b = multiply<torus>(instance.a, s);
    for(size_t i = 0; i < N; i++) {
        instance.b[i] += (m[i] + e[i]);
    }
    return instance;
}

// encrypt_polynomial_zero: zero message in T_N[X] -> trlwe
trlwe trlwe::encrypt_polynomial_zero(secret_key skey) {
    constexpr size_t N = params::N;
    std::array<torus, N> m;
    for(size_t i = 0; i < N; i++) {
        m[i] = 0;
    }
    return encrypt_polynomial_torus(skey, m);
}

// encrypt_polynomial_binary: message in B_N[X] -> trlwe
trlwe trlwe::encrypt_polynomial_binary(secret_key skey, std::array<bool, params::N> m) {
    const torus mu = 1 << (std::numeric_limits<torus>::digits - 3);
    constexpr size_t N = params::N;
    std::array<torus, N> m_binary;
    for(size_t i = 0; i < N; i++) {
        if(m[i]) {
            m_binary[i] = mu;
        } else {
            m_binary[i] = -mu;
        }
    }
    return encrypt_polynomial_torus(skey, m_binary);
}

// decrypt_polynomial_binary: trlwe -> message in B_N[X]
std::array<bool, params::N> trlwe::decrypt_polynomial_binary(secret_key skey) {
    constexpr size_t N = params::N;
    std::array<torus, N> s;
    for(size_t i = 0; i < N; i++) {
        s[i] = skey.lvl1[i];
    }

    std::array<int, N> m;
    std::array<torus, N> as = multiply<torus>(a, s);
    for(size_t i = 0; i < N; i++) {
        m[i] = b[i] - as[i];
    }

    std::array<bool, N> m_binary;
    for(size_t i = 0; i < N; i++) {
        if(m[i] > 0) {
            m_binary[i] = 1;
        } else {
            m_binary[i] = 0;
        }
    }
    return m_binary;
}

// SampleExtractIndex: trlwe -> tlwe_lvl1 (=tlwe_lvl0^N)
tlwe_lvl1 trlwe::sample_extract_index(trlwe trlwe, size_t k) {
    constexpr size_t N = params::N;
    tlwe_lvl1 tlwe_lvl1;
    tlwe_lvl1.b = trlwe.b[k];
    for(size_t i = 0; i < N; i++) {
        if(i <= k) {
            tlwe_lvl1.a[i] = trlwe.a[k - i];
        } else {
            tlwe_lvl1.a[i] = -trlwe.a[N + k - i];
        }
    }
    return tlwe_lvl1;
}

// decomposition:a[X] in T_N[X] -> a_bar[X] in (Z_N[X])^l
std::array<std::array<int, params::N>, params::l> trlwe::decomposition(std::array<torus, params::N> a) {
    constexpr size_t N = params::N;
    constexpr size_t l = params::l;
    constexpr size_t Bgbit = params::Bgbit;
    constexpr torus Bg = params::Bg;

    torus roundoffset = 1 << (32 - l * Bgbit - 1);

    // naive impl
    std::array<std::array<int, N>, l> a_bar;
    std::array<std::array<torus, N>, l> a_hat;

    for(size_t i = 0; i < l; i++) {
        for(size_t j = 0; j < N; j++) {
            a_hat[i][j] = ((a[j] + roundoffset) >> (32 - Bgbit * (i + 1))) & (Bg - 1);
        }
    }

    int carry = 0;
    for(size_t j = 0; j < N; j++) {
        for(int i = l - 1; i >= 0; i--) {
            a_hat[i][j] += carry;
            if(a_hat[i][j] >= Bg / 2) {
                a_bar[i][j] = a_hat[i][j] - Bg;
                carry = 1;
            } else {
                a_bar[i][j] = a_hat[i][j];
                carry = 0;
            }
        }
    }
    return a_bar;
}