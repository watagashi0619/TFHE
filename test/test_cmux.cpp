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
    bool message_c = 1;

    trlwe t = trlwe::encrypt_polynomial_binary(skey, message_t);
    trlwe f = trlwe::encrypt_polynomial_binary(skey, message_f);
    trgsw c = trgsw::encrypt_binary(skey, message_c);

    trlwe res = cmux(c, t, f);

    int cnt = 0;

    std::array<bool, params::N> res_plain = res.decrypt_polynomial_binary(skey);
    for(size_t i = 0; i < params::N; i++) {
        if((message_c ? message_t[i] : message_f[i]) == res_plain[i]) {
            std::cout << "pass" << std::endl;
            std::cout << message_t[i] << " " << message_f[i] << " " << res_plain[i] << std::endl;
        } else {
            std::cout << "failed" << std::endl;
            std::cout << message_t[i] << " " << message_f[i] << " " << res_plain[i] << std::endl;
            cnt++;
        }
    }
    std::cout << cnt << " " << params::N << std::endl;
}

int main() {
    test_cmux();
}