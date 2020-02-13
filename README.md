# C++20 Compile-Time SHA-1 and SHA-2 Hash Algorithms
This repository contains a header-only library allowing compile-time only (`consteval`) calculation of SHA-1, SHA-224,
SHA-256, SHA-384, SHA-512, SHA-512/224, and SHA-512/256 digests as defined in
[FIPS 180-4](https://nvlpubs.nist.gov/nistpubs/FIPS/NIST.FIPS.180-4.pdf). This library is restricted to compile-time
only because there are _much_ more efficient libraries to provide runtime hash calculations. Use this only if you need
to calculate SHA hash digests at compile-time.

All constants are derived from first principles where possible, rather than just hardcoding the constants given by the
document, so one can see where the magic numbers come from. While the algorithm appears to give the right answers, I
offer no guarantee that there are no bugs. Use this library at your own risk.

This implementation is exceptionally inefficient, and the tests take a long time to run. Performance was not really a
goal of the implementation; rather, it was an exercise in learning the `constexpr`, and `consteval` features in C++ and
an attempt to write the SHA family of functions in as concise a way as possible without hardcoding any magic numbers.

It is named "ctsha" for "Compile-Time SHA".

# Usage
Usage is fairly simple. Each hash function simply takes a `std::array<std::byte, N>` containing the message, and returns
a `std::array<std::byte, N>` containing the digest.

```c++
#include "ctsha.hpp"

constexpr auto data_to_hash = std::array{ std::byte{'a'}, std::byte{'b'}, std::byte{'c'} };

constexpr auto sha1_result       = ctsha::sha1(data_to_hash);
constexpr auto sha224_result     = ctsha::sha224(data_to_hash);
constexpr auto sha256_result     = ctsha::sha256(data_to_hash);
constexpr auto sha384_result     = ctsha::sha384(data_to_hash);
constexpr auto sha512_result     = ctsha::sha512(data_to_hash);
constexpr auto sha512_224_result = ctsha::sha512_t<224>(data_to_hash);
constexpr auto sha512_256_result = ctsha::sha512_t<256>(data_to_hash);
```

Some user-defined literals are provided to calculate the hash of a string more easily. The above example can be
accomplished using literals:

```c++
#include "ctsha.hpp"

using namespace ctsha::literals;

constexpr auto sha1_result       = "abc"_sha1;
constexpr auto sha224_result     = "abc"_sha224;
constexpr auto sha256_result     = "abc"_sha256;
constexpr auto sha384_result     = "abc"_sha384;
constexpr auto sha512_result     = "abc"_sha512;
constexpr auto sha512_224_result = "abc"_sha512_224;
constexpr auto sha512_256_result = "abc"_sha512_256;
```

Only the official SHA-512 truncations SHA-512/224 and SHA-512/256 have user-defined literals. Other trunctions are
possible using the longer `ctsha::sha512_t<123>(...)` syntax. These trunctions should work, but there are not test
vectors to validate them against, so caveat emptor.

# Tests
All tests are performed at compile-time with `static_assert` statements. The `test` BASH script executed in the
repository root will run some sanity tests contained in `ctsha_tests.cpp`, and if those pass it will download some
[test vectors](https://csrc.nist.gov/Projects/Cryptographic-Algorithm-Validation-Program/Secure-Hashing) and attempt to
run through all of those. These tests take a very long time and a very large amount of memory to run, but they do pass
successfully if you have sufficient memory.

A `Dockerfile` is provided that creates a Docker container that runs the tests in a known good environment. To run the
tests in a Docker container, run the following commands:

```bash
docker build . --tag ctsha
docker run --rm ctsha
```
