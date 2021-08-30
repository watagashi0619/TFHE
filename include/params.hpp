#pragma once
#include <cstdint>

namespace params {
// TLWE params
constexpr double alpha = 3.0517578125e-05;  // 2^(-15)
constexpr uint32_t n = 635;

// TRLWE Params
constexpr uint32_t N = 1024;
constexpr double alpha_bk = 2.98023223876953125e-08;  // 2^(-25) a_bootstrapping_key

// TRGSW Params
constexpr uint32_t Bgbit = 6;
constexpr uint32_t Bg = 1 << 6;
constexpr uint32_t l = 3;

}  // namespace params