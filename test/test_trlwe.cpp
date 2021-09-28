#include <array>
#include <cassert>
#include <iostream>
#include <random>

#include "key.hpp"
#include "params.hpp"
#include "trlwe.hpp"
#include "util.hpp"

using namespace TFHE;

// trlwe test
void test_trlwe() {
    std::cout << "trlwe" << std::endl;

    // secret key
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
        assert(message[i] == res[i]);
    }
    std::cout << "pass" << std::endl;
}

int main() {
    test_trlwe();
}
