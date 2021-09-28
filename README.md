# TFHE

Torus Fully Homomorphic Encryption

work on [seccamp 2021](https://www.ipa.go.jp/jinzai/camp/2021/zenkoku2021_index.html)

## Run Hom NAND Test

```
git clone https://github.com/watagashi0619/TFHE.git
cd TFHE
make
./bin/test_hom_nand
```

The `make` command builds all tests. If you want to build only `test_hom_nand`, replace `make` with `make test_hom_nand`.

## Hom NAND Benchmark

* naive implementation
* run test 100 iterations (`./bin/bench_hom_nand`)
  * average : 48114.3 ms
  * std : 211.12 ms
* Machine Info
  * MacBook Air (2020)
  * macOS Big Sur 11.5.2
  * CPU: Intel Core i5-1030NG7 4-Core 1.1GHz/3.5GHz
  * Memory: 16GB 3733MHz LPDDR4X

## References

* [seccamp slides](https://nindanaoto.github.io)
* https://tfhe.github.io/tfhe/

## License

MIT