#include <array>
#include <iostream>

#include "bootstrapping_key.hpp"
#include "gatebootstrapping.hpp"
#include "key.hpp"
#include "params.hpp"
#include "tlwe.hpp"
#include "trgsw.hpp"
#include "trlwe.hpp"
#include "util.hpp"

using namespace TFHE;

void test_blind_rotate() {
    std::array<bool, 2> messages = {true, false};
    for(auto &message : messages) {
        // secret key
        secret_key skey;
        // bootstrapping key (note: require a lot of memory)
        std::unique_ptr<bootstrapping_key> bkey;
        bkey = std::make_unique<bootstrapping_key>(skey);
        // encrypt message to tlwe_lvl0
        tlwe_lvl0 test_tlwe_lvl0 = tlwe_lvl0::encrypt_binary(skey, message);
        // gatebootstrapping tlwe_lvl0 to tlwe_lvl1
        tlwe_lvl1 res_tlwe_lvl1 = gatebootstrapping_tlwe_to_tlwe(test_tlwe_lvl0, *bkey);
        // decrypt res
        bool res = res_tlwe_lvl1.decrypt_binary(skey);

        if(message == res) {
            std::cout << "pass" << std::endl;
        } else {
            std::cout << "FAILED!" << std::endl;
        }
    }
}

int main() {
    test_blind_rotate();
}