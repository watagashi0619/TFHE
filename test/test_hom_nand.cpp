#include <array>
#include <iostream>
#include <random>

#include "key.hpp"
#include "params.hpp"
#include "tlwe.hpp"
#include "trgsw.hpp"
#include "trlwe.hpp"
#include "util.hpp"

using namespace TFHE;

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

int main() {
    test_hom_nand();
}
