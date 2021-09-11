#include "key.hpp"

#include <random>

#include "params.hpp"
#include "util.hpp"

namespace TFHE {

secret_key::secret_key() {
    for(size_t i = 0; i < params::n; i++) {
        lvl0[i] = binary_uniform_distribution();
    }
    for(size_t i = 0; i < params::N; i++) {
        lvl1[i] = binary_uniform_distribution();
    }
}

}