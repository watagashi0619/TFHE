#include <array>
#include <random>

#include "bootstrapping_key.hpp"
#include "key.hpp"
#include "params.hpp"
#include "trgsw.hpp"
#include "trlwe.hpp"

namespace TFHE {

template <typename T, size_t N>
std::array<T, N> poly_mult_by_X_k(std::array<T, N> poly_in, size_t k) {
    std::array<T, N> poly_out;
    if(k < N) {
        // for(size_t i = 0; i < N; i++) {
        //    poly_out[i] = poly_in[(i - k) % N] * (i < k ? -1 : 1);
        // }
        for(size_t i = 0; i < N - k; i++) {
            poly_out[i + k] = poly_in[i];
        }
        for(size_t i = N - k; i < N; i++) {
            poly_out[i + k - N] = -poly_in[i];
        }
    } else {
        // for(size_t i = 0; i < N; i++) {
        //    poly_out[i] = poly_in[(i - k) % N] * (i < k - N ? 1 : -1);
        //}
        for(size_t i = 0; i < 2 * N - k; i++) {
            poly_out[i + l] = -poly_in[i];
        }
        for(size_t i = 2 * N - k; i < N; i++) {
            poly_out[i + k - 2 * N] = poly_in[i];
        }
    }
    return poly_out;
}

trlwe blind_rotate(tlwe_lvl0 tlwe, bootstrapping_key& bkey, trlwe test_vector) {
    constexpr size_t Nbit = params::Nbit;
    constexpr size_t n = params::n;
    constexpr size_t N = params::N;

    size_t b_tilde = 2 * N - ((tlwe.b + (1 << (31 - Nbit - 1))) >> (32 - Nbit - 1));
    trlwe instance = trlwe();
    instance.a = poly_mult_by_X_k(test_vector.a, b_tilde);
    instance.b = poly_mult_by_X_k(test_vector.b, b_tilde);
    for(size_t i = 0; i < n; i++) {
        size_t a_tilde = (tlwe.a[i] + (1 << (31 - Nbit - 1))) >> (32 - Nbit - 1);
        trlwe instance_mult_by_X_a_tilde = trlwe();
        instance_mult_by_X_a_tilde.a = poly_mult_by_X_k(instance.a, a_tilde);
        instance_mult_by_X_a_tilde.b = poly_mult_by_X_k(instance.b, a_tilde);
        instance = cmux(bkey[i], instance_mult_by_X_a_tilde, instance);
    }
    return instance;
}

trlwe test_vector() {
    constexpr size_t N = params::N;
    constexpr torus mu = 1 << (32 - 3);

    trlwe instance = trlwe();
    std::array<torus, N> m;
    for(size_t i = 0; i < N; i++) {
        m[i] = mu;
    }
    for(size_t i = 0; i < N; i++) {
        instance.a[i] = 0;
    }
    instance.b = m;
    return instance;
}

tlwe_lvl1 gatebootstrapping_tlwe_to_tlwe(tlwe_lvl0 tlwe, bootstrapping_key& bkey) {
    trlwe test_vec = test_vector();
    trlwe trlwe = blind_rotate(tlwe, bkey, test_vec);
    return sample_extract_index(trlwe, 0);
}

}  // namespace TFHE