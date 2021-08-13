#include <array>
#include <iostream>
#include <random>

using torus = uint32_t;

namespace params {
// TLWE params
constexpr double alpha = 3.0517578125e-05;  // 2^(-15)
constexpr uint32_t n = 635;

// TRLWE Params
constexpr uint32_t N = 1024;
constexpr double alpha_bk = 2.98023223876953125e-08;  // 2^(-25) a_bootstrapping_key
}  // namespace params

std::random_device rng;

torus double_to_torus(double d) {
    return static_cast<torus>(std::fmod(d, 1.0) * std::pow(2.0, std::numeric_limits<torus>::digits));
}

// uniform distribution in torus
torus torus_uniform_distribution() {
    std::uniform_int_distribution<torus> dist(0, std::numeric_limits<torus>::max());
    return dist(rng);
}

// modular nomal distribution D_T,alpha
torus modular_normal_distribution(double alpha) {
    std::normal_distribution<> dist(0.0, alpha);
    return double_to_torus(dist(rng));
}

// uniform distribution in binary
bool binary_uniform_distribution() {
    std::uniform_int_distribution<> dist(0, 1);
    return dist(rng);
}

// secret key s in B^n
struct secret_key {
    std::array<bool, params::n> lvl0;
    std::array<bool, params::N> lvl1;
    secret_key() {
        for(size_t i = 0; i < params::n; i++) {
            lvl0[i] = binary_uniform_distribution();
        }
        for(size_t i = 0; i < params::N; i++) {
            lvl1[i] = binary_uniform_distribution();
        }
    }
};

// tlwe (a,b) in (R^n,R)
template <int lvl>
struct tlwe {
    static constexpr size_t N() noexcept {
        if constexpr(lvl == 0)
            return params::n;
        else
            return params::N;
    }

    std::array<torus, N()> a;
    torus b;

    static struct tlwe encrypt_torus(secret_key skey, torus m) {
        tlwe tlwe;
        for(size_t i = 0; i < N(); i++) {
            tlwe.a[i] = torus_uniform_distribution();
        }
        torus e = modular_normal_distribution(params::alpha);
        tlwe.b = m + e;
        for(size_t i = 0; i < N(); i++) {
            tlwe.b += tlwe.a[i] * skey.lvl0[i];
        }
        return tlwe;
    }

    static struct tlwe encrypt_binary(secret_key skey, bool m) {
        const torus mu = 1 << (std::numeric_limits<torus>::digits - 3);
        if(m) {
            return encrypt_torus(skey, mu);
        } else {
            return encrypt_torus(skey, 7 * mu);  // -mu == 7* mu in Torus
        }
    }

    bool decrypt_binary(secret_key skey) {
        int32_t m = b;
        for(size_t i = 0; i < N(); i++) {
            m -= a[i] * skey.lvl0[i];
        }
        return m > 0;
    }
};

using tlwe_lvl0 = tlwe<0>;
using tlwe_lvl1 = tlwe<1>;

// multiply in polynomial ring (naive)

std::array<torus, params::N> multiply(std::array<torus, params::N> a, std::array<torus, params::N> b) {
    size_t N = params::N;
    std::array<torus, params::N> c;
    for(size_t i = 0; i < N; i++) {
        c[i] = 0;
    }
    for(size_t i = 0; i < N; i++) {
        for(size_t j = 0; j < N; j++) {
            if(i + j < N) {
                c[i + j] += a[i] * b[j];
            } else {
                c[i + j - N] -= a[i] * b[j];
            }
        }
    }
    return c;
}

struct trlwe {
    std::array<torus, params::N> a, b;

    static trlwe encrypt_polynomial_torus(secret_key skey, std::array<torus, params::N> m) {
        constexpr size_t N = params::N;
        trlwe trlwe;
        const torus mu = 1 << (std::numeric_limits<torus>::digits - 3);
        for(size_t i = 0; i < N; i++) {
            trlwe.a[i] = torus_uniform_distribution();
        }
        std::array<torus, N> e;
        for(size_t i = 0; i < N; i++) {
            e[i] = modular_normal_distribution(params::alpha_bk);
        }
        std::array<torus, N> s;
        for(size_t i = 0; i < N; i++) {
            s[i] = skey.lvl1[i];
        }
        trlwe.b = multiply(trlwe.a, s);
        for(size_t i = 0; i < N; i++) {
            trlwe.b[i] += (m[i] + e[i]);
        }
        return trlwe;
    }

    static struct trlwe encrypt_polynomial_binary(secret_key skey, std::array<bool, params::N> m) {
        const torus mu = 1 << (std::numeric_limits<torus>::digits - 3);
        constexpr size_t N = params::N;
        std::array<torus, N> m_binary;
        for(size_t i = 0; i < N; i++) {
            if(m[i]) {
                m_binary[i] = mu;
            } else {
                m_binary[i] = mu * 7;  // -mu;
            }
        }
        return encrypt_polynomial_torus(skey, m_binary);
    }

    std::array<bool, params::N> decrypt_polynomial_binary(secret_key skey) {
        std::array<torus, params::N> s;
        for(size_t i = 0; i < params::N; i++) {
            s[i] = skey.lvl1[i];
        }

        std::array<torus, params::N> m;
        std::array<torus, params::N> as = multiply(a, s);
        for(size_t i = 0; i < params::N; i++) {
            m[i] = b[i] - as[i];
        }

        std::array<bool, params::N> m_binary;
        for(size_t i = 0; i < params::N; i++) {
            if(m[i] > 0) {
                m_binary[i] = 1;
            } else {
                m_binary[i] = 0;
            }
        }
        return m_binary;
    }
};

// SampleExtractIndex
tlwe_lvl1 sample_extract_index(trlwe trlwe, size_t k) {
    constexpr size_t N = params::N;
    tlwe_lvl1 tlwe_lvl1;
    tlwe_lvl1.b = trlwe.b[k];
    for(size_t i = 0; i < params::N; i++) {
        if(i <= k) {
            tlwe_lvl1.a[i] = trlwe.a[k - i];
        } else {
            tlwe_lvl1.a[i] = -trlwe.a[N + k - i];
        }
    }
    return tlwe_lvl1;
}

// hom_nand
tlwe_lvl0 hom_nand(tlwe_lvl0 tlwe1, tlwe_lvl0 tlwe2) {
    size_t n = params::n;
    const torus mu = 1 << (std::numeric_limits<torus>::digits - 3);
    tlwe_lvl0 tlwe;
    for(size_t i = 0; i < n; i++) {
        tlwe.a[i] = -(tlwe1.a[i] + tlwe2.a[i]);
    }
    tlwe.b = mu - (tlwe1.b + tlwe2.b);
    return tlwe;
}

// main (hom_nand test)
int main() {
    std::cout << "hom_nand" << std::endl;
    for(size_t i = 0; i <= 1; i++) {
        for(size_t j = 0; j <= 1; j++) {
            secret_key skey;
            auto tlwe1 = tlwe_lvl0::encrypt_binary(skey, i);
            auto tlwe2 = tlwe_lvl0::encrypt_binary(skey, j);
            auto tlwe_ = hom_nand(tlwe1, tlwe2);
            std::cout << tlwe1.decrypt_binary(skey) << "|" << tlwe2.decrypt_binary(skey) << "|" << tlwe_.decrypt_binary(skey) << std::endl;
        }
    }
}