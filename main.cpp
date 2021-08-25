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

// TRGSW Params
constexpr uint32_t Bgbit = 6;
constexpr uint32_t Bg = 1 << 6;
constexpr uint32_t l = 3;

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

// secret key lvl0 in B^n, lvl1 in B^N
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

// tlwe_lvl0 (a,b) in (R^n,R), tlwe_lvl1 (a,b) in (R^N,R)
template <int lvl>
struct tlwe {
    static constexpr size_t N() noexcept {
        if constexpr(lvl == 0) {
            return params::n;
        } else {
            return params::N;
        }
    }

    static const std::array<bool, N()> key(secret_key skey) noexcept {
        if constexpr(lvl == 0) {
            return skey.lvl0;
        } else {
            return skey.lvl1;
        }
    }

    std::array<torus, N()> a;
    torus b;

    static struct tlwe encrypt_torus(secret_key skey, torus m) {
        tlwe tlwe;
        auto s = key(skey);
        for(size_t i = 0; i < N(); i++) {
            tlwe.a[i] = torus_uniform_distribution();
        }
        torus e = modular_normal_distribution(params::alpha);
        tlwe.b = m + e;
        for(size_t i = 0; i < N(); i++) {
            tlwe.b += tlwe.a[i] * s[i];
        }
        return tlwe;
    }

    static struct tlwe encrypt_binary(secret_key skey, bool m) {
        const torus mu = 1 << (std::numeric_limits<torus>::digits - 3);
        if(m) {
            return encrypt_torus(skey, mu);
        } else {
            return encrypt_torus(skey, -mu);
        }
    }

    bool decrypt_binary(secret_key skey) {
        int32_t m = b;
        auto s = key(skey);
        for(size_t i = 0; i < N(); i++) {
            m -= a[i] * s[i];
        }
        return m > 0;
    }
};

using tlwe_lvl0 = tlwe<0>;
using tlwe_lvl1 = tlwe<1>;

// multiply in polynomial ring (naive)
// multiply: (T_N[X],T_N[X]) -> T_N[X]
template <typename T, typename T1, typename T2>
std::array<T, params::N> multiply(std::array<T1, params::N> a, std::array<T2, params::N> b) {
    size_t N = params::N;
    std::array<T, params::N> c;
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

// TRLWE
// (a,b) in (T_N[X],T_N[X])
struct trlwe {
    std::array<torus, params::N> a, b;

    // encrypt_polynomial_torus: message in T_N[X] -> trlwe
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
        trlwe.b = multiply<torus>(trlwe.a, s);
        for(size_t i = 0; i < N; i++) {
            trlwe.b[i] += (m[i] + e[i]);
        }
        return trlwe;
    }

    // encrypt_polynomial_zero: zero message in T_N[X] -> trlwe
    static trlwe encrypt_polynomial_zero(secret_key skey) {
        constexpr size_t N = params::N;
        std::array<torus, N> m;
        for(size_t i = 0; i < N; i++) {
            m[i] = 0;
        }
        return encrypt_polynomial_torus(skey, m);
    }

    // encrypt_polynomial_binary: message in B_N[X] -> trlwe
    static struct trlwe encrypt_polynomial_binary(secret_key skey, std::array<bool, params::N> m) {
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
    std::array<bool, params::N> decrypt_polynomial_binary(secret_key skey) {
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
};

// SampleExtractIndex: trlwe -> tlwe_lvl1 (=tlwe_lvl0^N)
tlwe_lvl1 sample_extract_index(trlwe trlwe, size_t k) {
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

// TRGSW
// trlwe^(2l) = [(a_0[X],b_0[X]),...,(a_{2l-1}[X],b_{2l-1}[X])]
struct trgsw {
    std::array<trlwe, 2 * params::l> secret_message;

    trlwe &operator[](size_t i) { return secret_message[i]; }

    const trlwe &operator[](size_t i) const { return secret_message[i]; }

    // encryot_polynomial_int: mu in Z_N[X] -> trgsw
    static trgsw encrypt_polynomial_int(secret_key skey, std::array<int, params::N> mu) {
        constexpr size_t N = params::N;
        constexpr size_t l = params::l;
        constexpr size_t Bgbit = params::Bgbit;

        trgsw trgsw;

        for(size_t i = 0; i < 2 * l; i++) {
            trgsw[i] = trlwe::encrypt_polynomial_zero(skey);
        }

        // trgsw[i].a += mu[X]/Bg^(i+1)
        // trgsw[i+l].b += mu[X]/Bg^(i+1) 0<=i<=l-1
        // mu[X]/Bg^(i+1)=x mod 2^32-1
        // mu[X]=x Bg^(i+1) mod 2^32-1
        // mu[X] 2^32 Bg^(-i-1)=x mod 2^32-1
        for(size_t i = 0; i < l; i++) {
            for(size_t j = 0; j < N; j++) {
                torus t = static_cast<torus>(mu[j]) * (1u << (32 - Bgbit * (i + 1)));
                trgsw[i].a[j] += t;
                trgsw[i + l].b[j] += t;
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
        return trgsw;
    }
};

// decomposition:a[X] in T_N[X] -> a_bar[X] in (Z_N[X])^l
std::array<std::array<int, params::N>, params::l> decomposition(std::array<torus, params::N> a) {
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

// external product: (trgsw,trlwe) -> trlwe
trlwe external_product(trgsw trgsw, trlwe trlwe_in) {
    constexpr size_t N = params::N;
    constexpr size_t l = params::l;
    constexpr size_t Bgbit = params::Bgbit;
    constexpr torus Bg = params::Bg;
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
    trlwe trlwe_tmp;
    for(size_t j = 0; j < N; j++) {
        trlwe_tmp.a[j] = trlwe1.a[j] - trlwe0.a[j];
        trlwe_tmp.b[j] = trlwe1.b[j] - trlwe0.b[j];
    }
    trlwe trlwe_out = external_product(trgsw, trlwe_tmp);
    for(size_t j = 0; j < N; j++) {
        trlwe_out.a[j] += trlwe0.a[j];
        trlwe_out.b[j] += trlwe0.b[j];
    }
    return trlwe_out;
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

// hom_nand test
void test_hom_nand() {
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

// trlwe test
void test_trlwe() {
    std::cout << "trlwe" << std::endl;

    secret_key skey;

    // message
    std::array<bool, params::N> message;
    for(size_t i = 0; i < params::N; i++) {
        message[i] = binary_uniform_distribution();
    }

    // encrypt message to trlwe
    trlwe test_trlwe = trlwe::encrypt_polynomial_binary(skey, message);

    // decrypt trlwe
    std::array<bool, params::N> res = test_trlwe.decrypt_polynomial_binary(skey);

    for(size_t i = 0; i < params::N; i++) {
        // res should be equal to message
        if(message[i] != res[i]) {
            std::cout << "FAILED!" << std::endl;
            exit(0);
        }
    }
    std::cout << "pass" << std::endl;
}

// extermal product test
void test_external_product() {
    std::cout << "external_product" << std::endl;

    secret_key skey;

    // message (to be trlwe)
    std::array<bool, params::N> message;
    for(size_t i = 0; i < params::N; i++) {
        message[i] = binary_uniform_distribution();
    }

    // encrypt message to trlwe
    trlwe test_trlwe = trlwe::encrypt_polynomial_binary(skey, message);

    // mu "1" (to be trgsw)
    std::array<int, params::N> mu = {1};

    // encrypt mu to trgsw
    trgsw test_trgsw = trgsw::encrypt_polynomial_int(skey, mu);

    // res is the result of the external product
    trlwe res = external_product(test_trgsw, test_trlwe);

    // decrypt res
    std::array<bool, params::N> res_decrypt = res.decrypt_polynomial_binary(skey);

    for(size_t i = 0; i < params::N; i++) {
        // res should be equal to message
        if(message[i] != res_decrypt[i]) {
            std::cout << "FAILED!" << std::endl;
            exit(0);
        }
    }
    std::cout << "pass" << std::endl;
}

int main() {
    // test_hom_nand();
    // test_trlwe();
    test_external_product();
}