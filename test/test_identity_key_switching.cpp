#include <array>
#include <cassert>
#include <iostream>

#include "identity_key_switching.hpp"
#include "key.hpp"
#include "key_switching_key.hpp"
#include "params.hpp"
#include "tlwe.hpp"

using namespace TFHE;

void test_identity_key_switching() {
    std::array<bool, 2> messages = {true, false};
    for(auto &message : messages) {
        // secret key
        secret_key skey;

        // key switching key (note: require a lot of memory)
        std::unique_ptr<key_switching_key> ks;
        ks = std::make_unique<key_switching_key>(skey);

        // encrypt message to tlwe_lvl1
        tlwe_lvl1 test_tlwe_lvl1 = tlwe_lvl1::encrypt_binary(skey, message);

        // identity key switching
        tlwe_lvl0 res_tlwe_lvl0 = identity_key_switching(test_tlwe_lvl1, *ks);

        // decrypt res_tlwe_lvl0
        bool res = res_tlwe_lvl0.decrypt_binary(skey);

        // res should be equal to message
        assert(message == res);
        std::cout << "pass" << std::endl;
    }
}

int main() {
    test_identity_key_switching();
}