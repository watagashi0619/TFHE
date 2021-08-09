#include <array>
#include <iostream>
#include <random>

using torus = uint32_t;

constexpr double alpha = 3.0517578125e-05;  // 2^(-15)
constexpr uint32_t n = 635;

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
    std::array<bool, n> s;
    secret_key() {
        for(size_t i = 0; i < n; i++) {
            s[i] = binary_uniform_distribution();
        }
    }
};

// tlwe (a,b) in R^(n+1)
struct tlwe {
    std::array<torus, n> a;
    torus b;

    static struct tlwe encrypt_torus(secret_key skey, torus m) {
        tlwe tlwe;
        for(size_t i = 0; i < n; i++) {
            tlwe.a[i] = torus_uniform_distribution();
        }
        torus e = modular_normal_distribution(alpha);
        tlwe.b = m + e;
        for(size_t i = 0; i < n; i++) {
            tlwe.b += tlwe.a[i] * skey.s[i];
        }
        return tlwe;
    }

    static struct tlwe encrypt_binary(secret_key skey, bool m) {
        torus mu = 1 << (std::numeric_limits<torus>::digits - 3);
        if(m) {
            return encrypt_torus(skey, mu);
        } else {
            return encrypt_torus(skey, -mu);
        }
    }

    bool decrypt_binary(secret_key skey) {
        torus m = b;
        for(size_t i = 0; i < n; i++) {
            m -= a[i] * skey.s[i];
        }
        return static_cast<int32_t>(m) > 0;
    }
};

// hom_nand
tlwe hom_nand(tlwe tlwe1, tlwe tlwe2) {
    const torus mu = 1 << 29;
    tlwe tlwe;
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
            auto tlwe1 = tlwe::encrypt_binary(skey, i);
            auto tlwe2 = tlwe::encrypt_binary(skey, j);
            auto tlwe_ = hom_nand(tlwe1, tlwe2);
            std::cout << i << "|" << j << "|" << tlwe_.decrypt_binary(skey) << std::endl;
        }
    }
}