#include <array>
#include <iostream>
#include <random>

#include "key.hpp"
#include "params.hpp"
#include "tlwe.hpp"
#include "trgsw.hpp"
#include "trlwe.hpp"
#include "util.hpp"

// extermal product test
void test_external_product() {
    std::cout << "external_product" << std::endl;

    std::array<int, 2> a = {1, -1};

    for(auto &x : a) {
        std::cout << "test trgsw mu=" << x << std::endl;

        secret_key skey;

        // message (to be trlwe)
        std::array<bool, params::N> message;
        for(size_t i = 0; i < params::N; i++) {
            message[i] = binary_uniform_distribution();
        }

        // encrypt message to trlwe
        trlwe test_trlwe = trlwe::encrypt_polynomial_binary(skey, message);

        // mu "1" or "-1" (to be trgsw)
        std::array<int, params::N> mu = {x};

        // encrypt mu to trgsw
        trgsw test_trgsw = trgsw::encrypt_polynomial_int(skey, mu);

        // res is the result of the external product
        trlwe res = trgsw::external_product(test_trgsw, test_trlwe);

        // decrypt res
        std::array<bool, params::N> res_decrypt = res.decrypt_polynomial_binary(skey);

        for(size_t i = 0; i < params::N; i++) {
            // when mu=1, res should be equal to message
            // when mu=-1, res shouldn't be equal to message
            if(message[i] ^ res_decrypt[i] == (x > 0)) {
                std::cout << "FAILED!" << std::endl;
                exit(0);
            }
        }
        std::cout << "pass" << std::endl;
    }
}

int main() {
    test_external_product();
}
