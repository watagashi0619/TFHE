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

void test_cmux() {
    secret_key skey;
    std::array<bool, params::N> message_t, message_f;
    for(size_t i = 0; i < params::N; i++) {
        message_t[i] = binary_uniform_distribution();
        message_f[i] = binary_uniform_distribution();
    }
    std::array<bool, 2> messages_c = {true, false};
    for(auto &message_c : messages_c) {
        trlwe t = trlwe::encrypt_polynomial_binary(skey, message_t);
        trlwe f = trlwe::encrypt_polynomial_binary(skey, message_f);
        trgsw c = trgsw::encrypt_binary(skey, message_c);

        trlwe res = cmux(c, t, f);

        std::array<bool, params::N> res_plain = res.decrypt_polynomial_binary(skey);
        if((message_c ? message_t : message_f) == res_plain) {
            std::cout << "pass" << std::endl;
        } else {
            std::cout << "failed" << std::endl;
        }
    }
}

int main() {
    test_cmux();
}