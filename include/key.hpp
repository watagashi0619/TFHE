#pragma once
#include <array>
#include <random>

#include "util.hpp"
struct secret_key {
    std::array<bool, params::n> lvl0;
    std::array<bool, params::N> lvl1;
    secret_key();
};