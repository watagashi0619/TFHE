#include <array>
#include <cassert>
#include <chrono>
#include <iostream>
#include <random>

#include "bootstrapping_key.hpp"
#include "gatebootstrapping.hpp"
#include "hom_nand.hpp"
#include "identity_key_switching.hpp"
#include "key.hpp"
#include "key_switching_key.hpp"
#include "params.hpp"
#include "tlwe.hpp"
#include "util.hpp"

using namespace TFHE;

// hom_nand test
void test_hom_nand() {
    std::cout << "hom_nand" << std::endl;
    for(size_t i = 0; i <= 1; i++) {
        for(size_t j = 0; j <= 1; j++) {
            // secret key
            secret_key skey;

            // bootstrapping key (note: require a lot of memory)
            std::unique_ptr<bootstrapping_key> bkey;
            bkey = std::make_unique<bootstrapping_key>(skey);

            // key switching key (note: require a lot of memory)
            std::unique_ptr<key_switching_key> ks;
            ks = std::make_unique<key_switching_key>(skey);

            // input tlwe_lvl0
            tlwe_lvl0 tlwe_a = tlwe_lvl0::encrypt_binary(skey, i);
            tlwe_lvl0 tlwe_b = tlwe_lvl0::encrypt_binary(skey, j);

            std::chrono::system_clock::time_point start, end;
            // timer start
            start = std::chrono::system_clock::now();
            // hom nand
            tlwe_lvl0 res = hom_nand(tlwe_a, tlwe_b, *bkey, *ks);
            // timer stop
            end = std::chrono::system_clock::now();
            double elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
            // show result
            assert((tlwe_a.decrypt_binary(skey) & tlwe_b.decrypt_binary(skey)) != res.decrypt_binary(skey));
            std::cout << tlwe_a.decrypt_binary(skey) << "|" << tlwe_b.decrypt_binary(skey) << "|" << res.decrypt_binary(skey) << " " << elapsed << "[ms]" << std::endl;
        }
    }
}

int main() {
    test_hom_nand();
}
