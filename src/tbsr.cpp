#include "tbsr.hpp"

#include <array>

#include "gatebootstrapping.hpp"  // poly_mult_by_X_k
#include "key.hpp"
#include "params.hpp"
#include "tlwe.hpp"
#include "trgsw.hpp"
#include "trlwe.hpp"
#include "util.hpp"

namespace TFHE {

trlwe& tbsr::operator[](size_t i) {
    return data[i];
}

const trlwe& tbsr::operator[](size_t i) const {
    return data[i];
}

tbsr::tbsr(secret_key& skey) {
    constexpr size_t tbsr_width = params::tbsr_width;
    constexpr torus mu = 1 << (std::numeric_limits<torus>::digits - 2);

    for(size_t i = 0; i < tbsr_width; i++) {
        std::array<torus, params::N> m = {};
        for(size_t j = 0; j < (1 << tbsr_width); j++) {
            m[j] = ((j & (1 << i)) > 0) ? mu : -mu;
        }
        data[i] = trlwe::encrypt_polynomial_torus(skey, m);
    }
}

int tbsr::decrypt_tbsr(secret_key& skey) {
    constexpr size_t tbsr_width = params::tbsr_width;
    int res = 0;
    for(size_t i = 0; i < tbsr_width; i++) {
        tlwe_lvl1 tlwe_lvl1 = sample_extract_index(data[i], 0);
        res += (tlwe_lvl1.decrypt_binary(skey) << i);
    }
    return res;
}

// generate a set of trgsw representing integer
std::array<trgsw, params::tbsr_width> tbsr::gen_trgsw_set(secret_key& skey, int x) {
    constexpr size_t tbsr_width = params::tbsr_width;
    std::array<trgsw, tbsr_width> instance;
    for(size_t i = 0; i < tbsr_width; i++) {
        instance[i] = trgsw::encrypt_binary(skey, (1 & (x >> i)));
    }
    return instance;
}

// add
// input trgsw_set_a,trgsw_set_b
// EXAMPLE: when you want to input integer 5, you make integer 5 bitwise expression (1,0,1,0,...) and decrypt each bits to trgsw
tbsr tbsr::add(secret_key& skey, std::array<trgsw, params::tbsr_width>& trgsw_set_a, std::array<trgsw, params::tbsr_width>& trgsw_set_b) {
    constexpr size_t tbsr_width = params::tbsr_width;
    constexpr size_t N = params::N;
    tbsr instance = tbsr(skey);
    // trgsw_set_a
    for(size_t i = 0; i < tbsr_width; i++) {
        for(size_t j = 0; j < tbsr_width; j++) {
            trlwe instance_mult_by_X_exp_of_2;
            instance_mult_by_X_exp_of_2.a = poly_mult_by_X_k(instance[j].a, 2 * N - (1 << i));
            instance_mult_by_X_exp_of_2.b = poly_mult_by_X_k(instance[j].b, 2 * N - (1 << i));
            instance[j] = cmux(trgsw_set_a[i], instance_mult_by_X_exp_of_2, instance[j]);
        }
    }
    // trgsw_set_b
    for(size_t i = 0; i < tbsr_width; i++) {
        for(size_t j = 0; j < tbsr_width; j++) {
            trlwe instance_mult_by_X_exp_of_2;
            instance_mult_by_X_exp_of_2.a = poly_mult_by_X_k(instance[j].a, 2 * N - (1 << i));
            instance_mult_by_X_exp_of_2.b = poly_mult_by_X_k(instance[j].b, 2 * N - (1 << i));
            instance[j] = cmux(trgsw_set_b[i], instance_mult_by_X_exp_of_2, instance[j]);
        }
    }
    return instance;
}

}  // namespace TFHE