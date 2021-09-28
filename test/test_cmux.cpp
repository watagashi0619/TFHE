#include <array>
#include <cassert>
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
    // secret key
    secret_key skey;

    // messages
    std::array<bool, params::N> message_t, message_f;
    for(size_t i = 0; i < params::N; i++) {
        message_t[i] = binary_uniform_distribution();
        message_f[i] = binary_uniform_distribution();
    }
    std::array<bool, 2> messages_c = {true, false};

    for(auto &message_c : messages_c) {
        // encrypt messages to trlwe
        trlwe t = trlwe::encrypt_polynomial_binary(skey, message_t);
        trlwe f = trlwe::encrypt_polynomial_binary(skey, message_f);
        trgsw c = trgsw::encrypt_binary(skey, message_c);

        // CMux
        trlwe res = cmux(c, t, f);

        // decrypt res
        std::array<bool, params::N> res_plain = res.decrypt_polynomial_binary(skey);

        // if message_c is 1, res_plain should be equal to message_t
        // if message_c is 0, res_plain should be equal to message_f
        assert((message_c ? message_t : message_f) == res_plain);
        std::cout << "pass" << std::endl;
    }
}

int main() {
    test_cmux();
}