#include <array>
#include <cassert>
#include <chrono>
#include <fstream>
#include <iomanip>
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

// benchmark test
void bench_hom_nand() {
    std::chrono::system_clock::time_point startup_time, finish_time;
    startup_time = std::chrono::system_clock::now();
    time_t t = std::chrono::system_clock::to_time_t(startup_time);
    tm* lt = std::localtime(&t);

    constexpr size_t round_iter = 25;
    constexpr size_t patterns = 4;
    constexpr size_t iter_count = round_iter * patterns;

    std::ofstream ofs("bench_hom_nand.log");
    std::cout.rdbuf(ofs.rdbuf());

    std::cout << "Hom NAND benchmark test: " << iter_count << " iterations" << std::endl;
    std::cout << "This is the measurement time for Hom NAND calculation only." << std::endl;
    std::cout << "startup: " << std::put_time(lt, "%c") << std::endl;
    std::cout << "============================================================" << std::endl;
    std::cout << "iter_count\ta|b|HOM_NAND(a,b)\telapsed[ms]" << std::endl;

    std::array<double, iter_count> elapseds;

    for(size_t i = 0; i < iter_count; i++) {
        // secret key
        secret_key skey;

        // bootstrapping key (note: require a lot of memory)
        std::unique_ptr<bootstrapping_key> bkey;
        bkey = std::make_unique<bootstrapping_key>(skey);

        // key switching key (note: require a lot of memory)
        std::unique_ptr<key_switching_key> ks;
        ks = std::make_unique<key_switching_key>(skey);

        // set messages
        bool a = 1 & ((i / round_iter) >> 1);
        bool b = 1 & ((i / round_iter) >> 0);

        // input tlwe_lvl0
        tlwe_lvl0 tlwe_a = tlwe_lvl0::encrypt_binary(skey, a);
        tlwe_lvl0 tlwe_b = tlwe_lvl0::encrypt_binary(skey, b);

        std::chrono::system_clock::time_point start, end;
        // timer start
        start = std::chrono::system_clock::now();
        // hom nand
        tlwe_lvl0 res = hom_nand(tlwe_a, tlwe_b, *bkey, *ks);
        // timer stop
        end = std::chrono::system_clock::now();
        elapseds[i] = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        // show result
        assert((tlwe_a.decrypt_binary(skey) & tlwe_b.decrypt_binary(skey)) != res.decrypt_binary(skey));
        std::cout << std::setfill('0') << std::right << std::setw(3) << i + 1 << "\t";
        std::cout << tlwe_a.decrypt_binary(skey) << "|" << tlwe_b.decrypt_binary(skey) << "|" << res.decrypt_binary(skey) << "\t" << elapseds[i] << std::endl;
    }

    double mean = 0;
    for(size_t i = 0; i < iter_count; i++) {
        mean += elapseds[i];
    }
    mean /= iter_count;

    double std = 0;
    for(size_t i = 0; i < iter_count; i++) {
        std += (elapseds[i] - mean) * (elapseds[i] - mean);
    }
    std /= iter_count;
    std = sqrt(std);

    std::cout << "============================================================" << std::endl;
    std::cout << "mean: " << mean << "[ms]" << std::endl;
    std::cout << "std: " << std << "[ms]" << std::endl;

    finish_time = std::chrono::system_clock::now();
    t = std::chrono::system_clock::to_time_t(finish_time);
    lt = std::localtime(&t);
    std::cout << "finish: " << std::put_time(lt, "%c") << std::endl;
}

int main() {
    bench_hom_nand();
}
