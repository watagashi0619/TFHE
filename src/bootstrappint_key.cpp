#include <array>
#include <random>

#include "bootstrapping_key.hpp"
#include "key.hpp"
#include "params.hpp"
#include "trgsw.hpp"
namespace TFHE {

trgsw& bootstrapping_key::operator[](size_t i) {
    return data[i];
}

const trgsw& bootstrapping_key::operator[](size_t i) const {
    return data[i];
}

bootstrapping_key::bootstrapping_key(secret_key skey) {
    for(size_t i = 0; i < params::n; i++) {
        data[i] = trgsw::encrypt_binary(skey, skey.lvl0[i]);
    }
}

}  // namespace TFHE