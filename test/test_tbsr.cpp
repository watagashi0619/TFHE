#include <array>
#include <cassert>
#include <iostream>
#include <random>

#include "key.hpp"
#include "params.hpp"
#include "tbsr.hpp"
#include "tlwe.hpp"
#include "trgsw.hpp"
#include "trlwe.hpp"
#include "util.hpp"

using namespace TFHE;

void test_tbsr_add() {
    // secret key
    secret_key skey;
    // generate integer a and b
    std::uniform_int_distribution<int> dist(0, (1 << (params::tbsr_width - 1)));
    int a = dist(rng);
    int b = dist(rng);
    std::cout << a << " + " << b << " = " << std::flush;
    // encrypt integer a,b to trgsw_set trgsw_set_a,trgsw_set_b
    std::array<trgsw, params::tbsr_width> trgsw_set_a = tbsr::gen_trgsw_set(skey, a);
    std::array<trgsw, params::tbsr_width> trgsw_set_b = tbsr::gen_trgsw_set(skey, b);
    // add
    tbsr res = tbsr::add(skey, trgsw_set_a, trgsw_set_b);
    // decrypt res
    int ans = res.decrypt_tbsr(skey);
    // answer
    std::cout << ans << std::endl;
    // judgement
    assert(a + b == ans);
    std::cout << "pass" << std::endl;
}

int main() {
    test_tbsr_add();
}