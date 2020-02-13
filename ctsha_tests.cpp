#include "ctsha.hpp"
#include "ctsha_tests.hpp"

using namespace ctsha::literals;

// Test the "bits" template variable.
static_assert(ctsha::detail::bits<std::uint8_t>  ==  8);
static_assert(ctsha::detail::bits<std::uint16_t> == 16);
static_assert(ctsha::detail::bits<std::uint32_t> == 32);
static_assert(ctsha::detail::bits<std::uint64_t> == 64);

// Test the "bytes" template variable.
static_assert(ctsha::detail::bytes< 0> == 0);
static_assert(ctsha::detail::bytes< 1> == 1);
static_assert(ctsha::detail::bytes< 7> == 1);
static_assert(ctsha::detail::bytes< 8> == 1);
static_assert(ctsha::detail::bytes< 9> == 2);
static_assert(ctsha::detail::bytes<10> == 2);
static_assert(ctsha::detail::bytes<15> == 2);
static_assert(ctsha::detail::bytes<16> == 2);
static_assert(ctsha::detail::bytes<17> == 3);

// Test the "byte_swap" function.
static_assert(ctsha::detail::byte_swap(std::uint8_t {0x00000000000000ab}) == 0x00000000000000ab);
static_assert(ctsha::detail::byte_swap(std::uint16_t{0x000000000000abcd}) == 0x000000000000cdab);
static_assert(ctsha::detail::byte_swap(std::uint32_t{0x0000000089abcdef}) == 0x00000000efcdab89);
static_assert(ctsha::detail::byte_swap(std::uint64_t{0x0123456789abcdef}) == 0xefcdab8967452301);

// Test the "to_bytes" function.
static_assert(ctsha::detail::to_bytes<std::endian::big>   (std::array<std::uint16_t, 2>{0x0123, 0x4567}) ==
              "01234567"_hex_bytes);
static_assert(ctsha::detail::to_bytes<std::endian::little>(std::array<std::uint16_t, 2>{0x0123, 0x4567}) ==
              "23016745"_hex_bytes);

// Test the "float128_root" function. (Simple tests. Other tests later will test it more thoroughly.)
static_assert(ctsha::detail::float128_root( 9, 2) == 3);
static_assert(ctsha::detail::float128_root(27, 3) == 3);
static_assert(ctsha::detail::float128_root(81, 4) == 3);

// Test the "is_prime" function.
static_assert( ctsha::detail::is_prime( 2));
static_assert( ctsha::detail::is_prime( 3));
static_assert(!ctsha::detail::is_prime( 4));
static_assert( ctsha::detail::is_prime( 5));
static_assert(!ctsha::detail::is_prime( 6));
static_assert( ctsha::detail::is_prime( 7));
static_assert(!ctsha::detail::is_prime( 8));
static_assert(!ctsha::detail::is_prime( 9));
static_assert(!ctsha::detail::is_prime(10));
static_assert( ctsha::detail::is_prime(11));

// Test the "next_prime" function.
static_assert(ctsha::detail::next_prime(2) == 2);
static_assert(ctsha::detail::next_prime(3) == 3);
static_assert(ctsha::detail::next_prime(4) == 5);
static_assert(ctsha::detail::next_prime(5) == 5);
static_assert(ctsha::detail::next_prime(6) == 7);
static_assert(ctsha::detail::next_prime(7) == 7);
static_assert(ctsha::detail::next_prime(8) == 11);
static_assert(ctsha::detail::next_prime(9) == 11);

// Test the "prime" function.
static_assert(ctsha::detail::prime(0) ==  2);
static_assert(ctsha::detail::prime(1) ==  3);
static_assert(ctsha::detail::prime(2) ==  5);
static_assert(ctsha::detail::prime(3) ==  7);
static_assert(ctsha::detail::prime(4) == 11);
static_assert(ctsha::detail::prime(5) == 13);

// Make sure the SHA-1 constants were computed correctly. (FIPS 180-4 section 4.2.1)
static_assert(ctsha::detail::sha1_constants.at( 0) == 0x5a827999);
static_assert(ctsha::detail::sha1_constants.at( 1) == 0x5a827999);
static_assert(ctsha::detail::sha1_constants.at( 2) == 0x5a827999);
static_assert(ctsha::detail::sha1_constants.at( 3) == 0x5a827999);
static_assert(ctsha::detail::sha1_constants.at( 4) == 0x5a827999);
static_assert(ctsha::detail::sha1_constants.at( 5) == 0x5a827999);
static_assert(ctsha::detail::sha1_constants.at( 6) == 0x5a827999);
static_assert(ctsha::detail::sha1_constants.at( 7) == 0x5a827999);
static_assert(ctsha::detail::sha1_constants.at( 8) == 0x5a827999);
static_assert(ctsha::detail::sha1_constants.at( 9) == 0x5a827999);
static_assert(ctsha::detail::sha1_constants.at(10) == 0x5a827999);
static_assert(ctsha::detail::sha1_constants.at(11) == 0x5a827999);
static_assert(ctsha::detail::sha1_constants.at(12) == 0x5a827999);
static_assert(ctsha::detail::sha1_constants.at(13) == 0x5a827999);
static_assert(ctsha::detail::sha1_constants.at(14) == 0x5a827999);
static_assert(ctsha::detail::sha1_constants.at(15) == 0x5a827999);
static_assert(ctsha::detail::sha1_constants.at(16) == 0x5a827999);
static_assert(ctsha::detail::sha1_constants.at(17) == 0x5a827999);
static_assert(ctsha::detail::sha1_constants.at(18) == 0x5a827999);
static_assert(ctsha::detail::sha1_constants.at(19) == 0x5a827999);
static_assert(ctsha::detail::sha1_constants.at(20) == 0x6ed9eba1);
static_assert(ctsha::detail::sha1_constants.at(21) == 0x6ed9eba1);
static_assert(ctsha::detail::sha1_constants.at(22) == 0x6ed9eba1);
static_assert(ctsha::detail::sha1_constants.at(23) == 0x6ed9eba1);
static_assert(ctsha::detail::sha1_constants.at(24) == 0x6ed9eba1);
static_assert(ctsha::detail::sha1_constants.at(25) == 0x6ed9eba1);
static_assert(ctsha::detail::sha1_constants.at(26) == 0x6ed9eba1);
static_assert(ctsha::detail::sha1_constants.at(27) == 0x6ed9eba1);
static_assert(ctsha::detail::sha1_constants.at(28) == 0x6ed9eba1);
static_assert(ctsha::detail::sha1_constants.at(29) == 0x6ed9eba1);
static_assert(ctsha::detail::sha1_constants.at(30) == 0x6ed9eba1);
static_assert(ctsha::detail::sha1_constants.at(31) == 0x6ed9eba1);
static_assert(ctsha::detail::sha1_constants.at(32) == 0x6ed9eba1);
static_assert(ctsha::detail::sha1_constants.at(33) == 0x6ed9eba1);
static_assert(ctsha::detail::sha1_constants.at(34) == 0x6ed9eba1);
static_assert(ctsha::detail::sha1_constants.at(35) == 0x6ed9eba1);
static_assert(ctsha::detail::sha1_constants.at(36) == 0x6ed9eba1);
static_assert(ctsha::detail::sha1_constants.at(37) == 0x6ed9eba1);
static_assert(ctsha::detail::sha1_constants.at(38) == 0x6ed9eba1);
static_assert(ctsha::detail::sha1_constants.at(39) == 0x6ed9eba1);
static_assert(ctsha::detail::sha1_constants.at(40) == 0x8f1bbcdc);
static_assert(ctsha::detail::sha1_constants.at(41) == 0x8f1bbcdc);
static_assert(ctsha::detail::sha1_constants.at(42) == 0x8f1bbcdc);
static_assert(ctsha::detail::sha1_constants.at(43) == 0x8f1bbcdc);
static_assert(ctsha::detail::sha1_constants.at(44) == 0x8f1bbcdc);
static_assert(ctsha::detail::sha1_constants.at(45) == 0x8f1bbcdc);
static_assert(ctsha::detail::sha1_constants.at(46) == 0x8f1bbcdc);
static_assert(ctsha::detail::sha1_constants.at(47) == 0x8f1bbcdc);
static_assert(ctsha::detail::sha1_constants.at(48) == 0x8f1bbcdc);
static_assert(ctsha::detail::sha1_constants.at(49) == 0x8f1bbcdc);
static_assert(ctsha::detail::sha1_constants.at(50) == 0x8f1bbcdc);
static_assert(ctsha::detail::sha1_constants.at(51) == 0x8f1bbcdc);
static_assert(ctsha::detail::sha1_constants.at(52) == 0x8f1bbcdc);
static_assert(ctsha::detail::sha1_constants.at(53) == 0x8f1bbcdc);
static_assert(ctsha::detail::sha1_constants.at(54) == 0x8f1bbcdc);
static_assert(ctsha::detail::sha1_constants.at(55) == 0x8f1bbcdc);
static_assert(ctsha::detail::sha1_constants.at(56) == 0x8f1bbcdc);
static_assert(ctsha::detail::sha1_constants.at(57) == 0x8f1bbcdc);
static_assert(ctsha::detail::sha1_constants.at(58) == 0x8f1bbcdc);
static_assert(ctsha::detail::sha1_constants.at(59) == 0x8f1bbcdc);
static_assert(ctsha::detail::sha1_constants.at(60) == 0xca62c1d6);
static_assert(ctsha::detail::sha1_constants.at(61) == 0xca62c1d6);
static_assert(ctsha::detail::sha1_constants.at(62) == 0xca62c1d6);
static_assert(ctsha::detail::sha1_constants.at(63) == 0xca62c1d6);
static_assert(ctsha::detail::sha1_constants.at(64) == 0xca62c1d6);
static_assert(ctsha::detail::sha1_constants.at(65) == 0xca62c1d6);
static_assert(ctsha::detail::sha1_constants.at(66) == 0xca62c1d6);
static_assert(ctsha::detail::sha1_constants.at(67) == 0xca62c1d6);
static_assert(ctsha::detail::sha1_constants.at(68) == 0xca62c1d6);
static_assert(ctsha::detail::sha1_constants.at(69) == 0xca62c1d6);
static_assert(ctsha::detail::sha1_constants.at(70) == 0xca62c1d6);
static_assert(ctsha::detail::sha1_constants.at(71) == 0xca62c1d6);
static_assert(ctsha::detail::sha1_constants.at(72) == 0xca62c1d6);
static_assert(ctsha::detail::sha1_constants.at(73) == 0xca62c1d6);
static_assert(ctsha::detail::sha1_constants.at(74) == 0xca62c1d6);
static_assert(ctsha::detail::sha1_constants.at(75) == 0xca62c1d6);
static_assert(ctsha::detail::sha1_constants.at(76) == 0xca62c1d6);
static_assert(ctsha::detail::sha1_constants.at(77) == 0xca62c1d6);
static_assert(ctsha::detail::sha1_constants.at(78) == 0xca62c1d6);
static_assert(ctsha::detail::sha1_constants.at(79) == 0xca62c1d6);

// Make sure the SHA-1 initialization vector was computed correctly. (FIPS 180-4 section 5.3.1)
static_assert(ctsha::detail::sha1_initialization_vector.at(0) == 0x67452301);
static_assert(ctsha::detail::sha1_initialization_vector.at(1) == 0xefcdab89);
static_assert(ctsha::detail::sha1_initialization_vector.at(2) == 0x98badcfe);
static_assert(ctsha::detail::sha1_initialization_vector.at(3) == 0x10325476);
static_assert(ctsha::detail::sha1_initialization_vector.at(4) == 0xc3d2e1f0);

// Make sure the SHA-224 and SHA-256 constants were computed correctly. (FIPS 180-4 section 4.2.2)
static_assert(ctsha::detail::sha2_32_bit_constants.at( 0) == 0x428a2f98);
static_assert(ctsha::detail::sha2_32_bit_constants.at( 1) == 0x71374491);
static_assert(ctsha::detail::sha2_32_bit_constants.at( 2) == 0xb5c0fbcf);
static_assert(ctsha::detail::sha2_32_bit_constants.at( 3) == 0xe9b5dba5);
static_assert(ctsha::detail::sha2_32_bit_constants.at( 4) == 0x3956c25b);
static_assert(ctsha::detail::sha2_32_bit_constants.at( 5) == 0x59f111f1);
static_assert(ctsha::detail::sha2_32_bit_constants.at( 6) == 0x923f82a4);
static_assert(ctsha::detail::sha2_32_bit_constants.at( 7) == 0xab1c5ed5);
static_assert(ctsha::detail::sha2_32_bit_constants.at( 8) == 0xd807aa98);
static_assert(ctsha::detail::sha2_32_bit_constants.at( 9) == 0x12835b01);
static_assert(ctsha::detail::sha2_32_bit_constants.at(10) == 0x243185be);
static_assert(ctsha::detail::sha2_32_bit_constants.at(11) == 0x550c7dc3);
static_assert(ctsha::detail::sha2_32_bit_constants.at(12) == 0x72be5d74);
static_assert(ctsha::detail::sha2_32_bit_constants.at(13) == 0x80deb1fe);
static_assert(ctsha::detail::sha2_32_bit_constants.at(14) == 0x9bdc06a7);
static_assert(ctsha::detail::sha2_32_bit_constants.at(15) == 0xc19bf174);
static_assert(ctsha::detail::sha2_32_bit_constants.at(16) == 0xe49b69c1);
static_assert(ctsha::detail::sha2_32_bit_constants.at(17) == 0xefbe4786);
static_assert(ctsha::detail::sha2_32_bit_constants.at(18) == 0x0fc19dc6);
static_assert(ctsha::detail::sha2_32_bit_constants.at(19) == 0x240ca1cc);
static_assert(ctsha::detail::sha2_32_bit_constants.at(20) == 0x2de92c6f);
static_assert(ctsha::detail::sha2_32_bit_constants.at(21) == 0x4a7484aa);
static_assert(ctsha::detail::sha2_32_bit_constants.at(22) == 0x5cb0a9dc);
static_assert(ctsha::detail::sha2_32_bit_constants.at(23) == 0x76f988da);
static_assert(ctsha::detail::sha2_32_bit_constants.at(24) == 0x983e5152);
static_assert(ctsha::detail::sha2_32_bit_constants.at(25) == 0xa831c66d);
static_assert(ctsha::detail::sha2_32_bit_constants.at(26) == 0xb00327c8);
static_assert(ctsha::detail::sha2_32_bit_constants.at(27) == 0xbf597fc7);
static_assert(ctsha::detail::sha2_32_bit_constants.at(28) == 0xc6e00bf3);
static_assert(ctsha::detail::sha2_32_bit_constants.at(29) == 0xd5a79147);
static_assert(ctsha::detail::sha2_32_bit_constants.at(30) == 0x06ca6351);
static_assert(ctsha::detail::sha2_32_bit_constants.at(31) == 0x14292967);
static_assert(ctsha::detail::sha2_32_bit_constants.at(32) == 0x27b70a85);
static_assert(ctsha::detail::sha2_32_bit_constants.at(33) == 0x2e1b2138);
static_assert(ctsha::detail::sha2_32_bit_constants.at(34) == 0x4d2c6dfc);
static_assert(ctsha::detail::sha2_32_bit_constants.at(35) == 0x53380d13);
static_assert(ctsha::detail::sha2_32_bit_constants.at(36) == 0x650a7354);
static_assert(ctsha::detail::sha2_32_bit_constants.at(37) == 0x766a0abb);
static_assert(ctsha::detail::sha2_32_bit_constants.at(38) == 0x81c2c92e);
static_assert(ctsha::detail::sha2_32_bit_constants.at(39) == 0x92722c85);
static_assert(ctsha::detail::sha2_32_bit_constants.at(40) == 0xa2bfe8a1);
static_assert(ctsha::detail::sha2_32_bit_constants.at(41) == 0xa81a664b);
static_assert(ctsha::detail::sha2_32_bit_constants.at(42) == 0xc24b8b70);
static_assert(ctsha::detail::sha2_32_bit_constants.at(43) == 0xc76c51a3);
static_assert(ctsha::detail::sha2_32_bit_constants.at(44) == 0xd192e819);
static_assert(ctsha::detail::sha2_32_bit_constants.at(45) == 0xd6990624);
static_assert(ctsha::detail::sha2_32_bit_constants.at(46) == 0xf40e3585);
static_assert(ctsha::detail::sha2_32_bit_constants.at(47) == 0x106aa070);
static_assert(ctsha::detail::sha2_32_bit_constants.at(48) == 0x19a4c116);
static_assert(ctsha::detail::sha2_32_bit_constants.at(49) == 0x1e376c08);
static_assert(ctsha::detail::sha2_32_bit_constants.at(50) == 0x2748774c);
static_assert(ctsha::detail::sha2_32_bit_constants.at(51) == 0x34b0bcb5);
static_assert(ctsha::detail::sha2_32_bit_constants.at(52) == 0x391c0cb3);
static_assert(ctsha::detail::sha2_32_bit_constants.at(53) == 0x4ed8aa4a);
static_assert(ctsha::detail::sha2_32_bit_constants.at(54) == 0x5b9cca4f);
static_assert(ctsha::detail::sha2_32_bit_constants.at(55) == 0x682e6ff3);
static_assert(ctsha::detail::sha2_32_bit_constants.at(56) == 0x748f82ee);
static_assert(ctsha::detail::sha2_32_bit_constants.at(57) == 0x78a5636f);
static_assert(ctsha::detail::sha2_32_bit_constants.at(58) == 0x84c87814);
static_assert(ctsha::detail::sha2_32_bit_constants.at(59) == 0x8cc70208);
static_assert(ctsha::detail::sha2_32_bit_constants.at(60) == 0x90befffa);
static_assert(ctsha::detail::sha2_32_bit_constants.at(61) == 0xa4506ceb);
static_assert(ctsha::detail::sha2_32_bit_constants.at(62) == 0xbef9a3f7);
static_assert(ctsha::detail::sha2_32_bit_constants.at(63) == 0xc67178f2);

// Make sure the SHA-384 and SHA-512 constants were computed correctly. (FIPS 180-4 section 4.2.3)
static_assert(ctsha::detail::sha2_64_bit_constants.at( 0) == 0x428a2f98d728ae22);
static_assert(ctsha::detail::sha2_64_bit_constants.at( 1) == 0x7137449123ef65cd);
static_assert(ctsha::detail::sha2_64_bit_constants.at( 2) == 0xb5c0fbcfec4d3b2f);
static_assert(ctsha::detail::sha2_64_bit_constants.at( 3) == 0xe9b5dba58189dbbc);
static_assert(ctsha::detail::sha2_64_bit_constants.at( 4) == 0x3956c25bf348b538);
static_assert(ctsha::detail::sha2_64_bit_constants.at( 5) == 0x59f111f1b605d019);
static_assert(ctsha::detail::sha2_64_bit_constants.at( 6) == 0x923f82a4af194f9b);
static_assert(ctsha::detail::sha2_64_bit_constants.at( 7) == 0xab1c5ed5da6d8118);
static_assert(ctsha::detail::sha2_64_bit_constants.at( 8) == 0xd807aa98a3030242);
static_assert(ctsha::detail::sha2_64_bit_constants.at( 9) == 0x12835b0145706fbe);
static_assert(ctsha::detail::sha2_64_bit_constants.at(10) == 0x243185be4ee4b28c);
static_assert(ctsha::detail::sha2_64_bit_constants.at(11) == 0x550c7dc3d5ffb4e2);
static_assert(ctsha::detail::sha2_64_bit_constants.at(12) == 0x72be5d74f27b896f);
static_assert(ctsha::detail::sha2_64_bit_constants.at(13) == 0x80deb1fe3b1696b1);
static_assert(ctsha::detail::sha2_64_bit_constants.at(14) == 0x9bdc06a725c71235);
static_assert(ctsha::detail::sha2_64_bit_constants.at(15) == 0xc19bf174cf692694);
static_assert(ctsha::detail::sha2_64_bit_constants.at(16) == 0xe49b69c19ef14ad2);
static_assert(ctsha::detail::sha2_64_bit_constants.at(17) == 0xefbe4786384f25e3);
static_assert(ctsha::detail::sha2_64_bit_constants.at(18) == 0x0fc19dc68b8cd5b5);
static_assert(ctsha::detail::sha2_64_bit_constants.at(19) == 0x240ca1cc77ac9c65);
static_assert(ctsha::detail::sha2_64_bit_constants.at(20) == 0x2de92c6f592b0275);
static_assert(ctsha::detail::sha2_64_bit_constants.at(21) == 0x4a7484aa6ea6e483);
static_assert(ctsha::detail::sha2_64_bit_constants.at(22) == 0x5cb0a9dcbd41fbd4);
static_assert(ctsha::detail::sha2_64_bit_constants.at(23) == 0x76f988da831153b5);
static_assert(ctsha::detail::sha2_64_bit_constants.at(24) == 0x983e5152ee66dfab);
static_assert(ctsha::detail::sha2_64_bit_constants.at(25) == 0xa831c66d2db43210);
static_assert(ctsha::detail::sha2_64_bit_constants.at(26) == 0xb00327c898fb213f);
static_assert(ctsha::detail::sha2_64_bit_constants.at(27) == 0xbf597fc7beef0ee4);
static_assert(ctsha::detail::sha2_64_bit_constants.at(28) == 0xc6e00bf33da88fc2);
static_assert(ctsha::detail::sha2_64_bit_constants.at(29) == 0xd5a79147930aa725);
static_assert(ctsha::detail::sha2_64_bit_constants.at(30) == 0x06ca6351e003826f);
static_assert(ctsha::detail::sha2_64_bit_constants.at(31) == 0x142929670a0e6e70);
static_assert(ctsha::detail::sha2_64_bit_constants.at(32) == 0x27b70a8546d22ffc);
static_assert(ctsha::detail::sha2_64_bit_constants.at(33) == 0x2e1b21385c26c926);
static_assert(ctsha::detail::sha2_64_bit_constants.at(34) == 0x4d2c6dfc5ac42aed);
static_assert(ctsha::detail::sha2_64_bit_constants.at(35) == 0x53380d139d95b3df);
static_assert(ctsha::detail::sha2_64_bit_constants.at(36) == 0x650a73548baf63de);
static_assert(ctsha::detail::sha2_64_bit_constants.at(37) == 0x766a0abb3c77b2a8);
static_assert(ctsha::detail::sha2_64_bit_constants.at(38) == 0x81c2c92e47edaee6);
static_assert(ctsha::detail::sha2_64_bit_constants.at(39) == 0x92722c851482353b);
static_assert(ctsha::detail::sha2_64_bit_constants.at(40) == 0xa2bfe8a14cf10364);
static_assert(ctsha::detail::sha2_64_bit_constants.at(41) == 0xa81a664bbc423001);
static_assert(ctsha::detail::sha2_64_bit_constants.at(42) == 0xc24b8b70d0f89791);
static_assert(ctsha::detail::sha2_64_bit_constants.at(43) == 0xc76c51a30654be30);
static_assert(ctsha::detail::sha2_64_bit_constants.at(44) == 0xd192e819d6ef5218);
static_assert(ctsha::detail::sha2_64_bit_constants.at(45) == 0xd69906245565a910);
static_assert(ctsha::detail::sha2_64_bit_constants.at(46) == 0xf40e35855771202a);
static_assert(ctsha::detail::sha2_64_bit_constants.at(47) == 0x106aa07032bbd1b8);
static_assert(ctsha::detail::sha2_64_bit_constants.at(48) == 0x19a4c116b8d2d0c8);
static_assert(ctsha::detail::sha2_64_bit_constants.at(49) == 0x1e376c085141ab53);
static_assert(ctsha::detail::sha2_64_bit_constants.at(50) == 0x2748774cdf8eeb99);
static_assert(ctsha::detail::sha2_64_bit_constants.at(51) == 0x34b0bcb5e19b48a8);
static_assert(ctsha::detail::sha2_64_bit_constants.at(52) == 0x391c0cb3c5c95a63);
static_assert(ctsha::detail::sha2_64_bit_constants.at(53) == 0x4ed8aa4ae3418acb);
static_assert(ctsha::detail::sha2_64_bit_constants.at(54) == 0x5b9cca4f7763e373);
static_assert(ctsha::detail::sha2_64_bit_constants.at(55) == 0x682e6ff3d6b2b8a3);
static_assert(ctsha::detail::sha2_64_bit_constants.at(56) == 0x748f82ee5defb2fc);
static_assert(ctsha::detail::sha2_64_bit_constants.at(57) == 0x78a5636f43172f60);
static_assert(ctsha::detail::sha2_64_bit_constants.at(58) == 0x84c87814a1f0ab72);
static_assert(ctsha::detail::sha2_64_bit_constants.at(59) == 0x8cc702081a6439ec);
static_assert(ctsha::detail::sha2_64_bit_constants.at(60) == 0x90befffa23631e28);
static_assert(ctsha::detail::sha2_64_bit_constants.at(61) == 0xa4506cebde82bde9);
static_assert(ctsha::detail::sha2_64_bit_constants.at(62) == 0xbef9a3f7b2c67915);
static_assert(ctsha::detail::sha2_64_bit_constants.at(63) == 0xc67178f2e372532b);
static_assert(ctsha::detail::sha2_64_bit_constants.at(64) == 0xca273eceea26619c);
static_assert(ctsha::detail::sha2_64_bit_constants.at(65) == 0xd186b8c721c0c207);
static_assert(ctsha::detail::sha2_64_bit_constants.at(66) == 0xeada7dd6cde0eb1e);
static_assert(ctsha::detail::sha2_64_bit_constants.at(67) == 0xf57d4f7fee6ed178);
static_assert(ctsha::detail::sha2_64_bit_constants.at(68) == 0x06f067aa72176fba);
static_assert(ctsha::detail::sha2_64_bit_constants.at(69) == 0x0a637dc5a2c898a6);
static_assert(ctsha::detail::sha2_64_bit_constants.at(70) == 0x113f9804bef90dae);
static_assert(ctsha::detail::sha2_64_bit_constants.at(71) == 0x1b710b35131c471b);
static_assert(ctsha::detail::sha2_64_bit_constants.at(72) == 0x28db77f523047d84);
static_assert(ctsha::detail::sha2_64_bit_constants.at(73) == 0x32caab7b40c72493);
static_assert(ctsha::detail::sha2_64_bit_constants.at(74) == 0x3c9ebe0a15c9bebc);
static_assert(ctsha::detail::sha2_64_bit_constants.at(75) == 0x431d67c49c100d4c);
static_assert(ctsha::detail::sha2_64_bit_constants.at(76) == 0x4cc5d4becb3e42b6);
static_assert(ctsha::detail::sha2_64_bit_constants.at(77) == 0x597f299cfc657e2a);
static_assert(ctsha::detail::sha2_64_bit_constants.at(78) == 0x5fcb6fab3ad6faec);
static_assert(ctsha::detail::sha2_64_bit_constants.at(79) == 0x6c44198c4a475817);

// Make sure the SHA-224 initialization vector was computed correctly. (FIPS 180-4 section 5.3.2)
static_assert(ctsha::detail::sha224_initialization_vector.at(0) == 0xc1059ed8);
static_assert(ctsha::detail::sha224_initialization_vector.at(1) == 0x367cd507);
static_assert(ctsha::detail::sha224_initialization_vector.at(2) == 0x3070dd17);
static_assert(ctsha::detail::sha224_initialization_vector.at(3) == 0xf70e5939);
static_assert(ctsha::detail::sha224_initialization_vector.at(4) == 0xffc00b31);
static_assert(ctsha::detail::sha224_initialization_vector.at(5) == 0x68581511);
static_assert(ctsha::detail::sha224_initialization_vector.at(6) == 0x64f98fa7);
static_assert(ctsha::detail::sha224_initialization_vector.at(7) == 0xbefa4fa4);

// Make sure the SHA-256 initialization vector was computed correctly. (FIPS 180-4 section 5.3.3)
static_assert(ctsha::detail::sha256_initialization_vector.at(0) == 0x6a09e667);
static_assert(ctsha::detail::sha256_initialization_vector.at(1) == 0xbb67ae85);
static_assert(ctsha::detail::sha256_initialization_vector.at(2) == 0x3c6ef372);
static_assert(ctsha::detail::sha256_initialization_vector.at(3) == 0xa54ff53a);
static_assert(ctsha::detail::sha256_initialization_vector.at(4) == 0x510e527f);
static_assert(ctsha::detail::sha256_initialization_vector.at(5) == 0x9b05688c);
static_assert(ctsha::detail::sha256_initialization_vector.at(6) == 0x1f83d9ab);
static_assert(ctsha::detail::sha256_initialization_vector.at(7) == 0x5be0cd19);

// Make sure the SHA-384 initialization vector was computed correctly. (FIPS 180-4 section 5.3.4)
static_assert(ctsha::detail::sha384_initialization_vector.at(0) == 0xcbbb9d5dc1059ed8);
static_assert(ctsha::detail::sha384_initialization_vector.at(1) == 0x629a292a367cd507);
static_assert(ctsha::detail::sha384_initialization_vector.at(2) == 0x9159015a3070dd17);
static_assert(ctsha::detail::sha384_initialization_vector.at(3) == 0x152fecd8f70e5939);
static_assert(ctsha::detail::sha384_initialization_vector.at(4) == 0x67332667ffc00b31);
static_assert(ctsha::detail::sha384_initialization_vector.at(5) == 0x8eb44a8768581511);
static_assert(ctsha::detail::sha384_initialization_vector.at(6) == 0xdb0c2e0d64f98fa7);
static_assert(ctsha::detail::sha384_initialization_vector.at(7) == 0x47b5481dbefa4fa4);

// Make sure the SHA-512 initialization vector was computed correctly. (FIPS 180-4 section 5.3.5)
static_assert(ctsha::detail::sha512_initialization_vector.at(0) == 0x6a09e667f3bcc908);
static_assert(ctsha::detail::sha512_initialization_vector.at(1) == 0xbb67ae8584caa73b);
static_assert(ctsha::detail::sha512_initialization_vector.at(2) == 0x3c6ef372fe94f82b);
static_assert(ctsha::detail::sha512_initialization_vector.at(3) == 0xa54ff53a5f1d36f1);
static_assert(ctsha::detail::sha512_initialization_vector.at(4) == 0x510e527fade682d1);
static_assert(ctsha::detail::sha512_initialization_vector.at(5) == 0x9b05688c2b3e6c1f);
static_assert(ctsha::detail::sha512_initialization_vector.at(6) == 0x1f83d9abfb41bd6b);
static_assert(ctsha::detail::sha512_initialization_vector.at(7) == 0x5be0cd19137e2179);

// Make sure the SHA-512/224 initialization vector was computed correctly. (FIPS 180-4 section 5.3.6.1)
static_assert(ctsha::detail::sha512_t_initialization_vector<224>.at(0) == 0x8C3D37C819544DA2);
static_assert(ctsha::detail::sha512_t_initialization_vector<224>.at(1) == 0x73E1996689DCD4D6);
static_assert(ctsha::detail::sha512_t_initialization_vector<224>.at(2) == 0x1DFAB7AE32FF9C82);
static_assert(ctsha::detail::sha512_t_initialization_vector<224>.at(3) == 0x679DD514582F9FCF);
static_assert(ctsha::detail::sha512_t_initialization_vector<224>.at(4) == 0x0F6D2B697BD44DA8);
static_assert(ctsha::detail::sha512_t_initialization_vector<224>.at(5) == 0x77E36F7304C48942);
static_assert(ctsha::detail::sha512_t_initialization_vector<224>.at(6) == 0x3F9D85A86A1D36C8);
static_assert(ctsha::detail::sha512_t_initialization_vector<224>.at(7) == 0x1112E6AD91D692A1);

// Make sure the SHA-512/256 initialization vector was computed correctly. (FIPS 180-4 section 5.3.6.2)
static_assert(ctsha::detail::sha512_t_initialization_vector<256>.at(0) == 0x22312194FC2BF72C);
static_assert(ctsha::detail::sha512_t_initialization_vector<256>.at(1) == 0x9F555FA3C84C64C2);
static_assert(ctsha::detail::sha512_t_initialization_vector<256>.at(2) == 0x2393B86B6F53B151);
static_assert(ctsha::detail::sha512_t_initialization_vector<256>.at(3) == 0x963877195940EABD);
static_assert(ctsha::detail::sha512_t_initialization_vector<256>.at(4) == 0x96283EE2A88EFFE3);
static_assert(ctsha::detail::sha512_t_initialization_vector<256>.at(5) == 0xBE5E1E2553863992);
static_assert(ctsha::detail::sha512_t_initialization_vector<256>.at(6) == 0x2B0199FC2C85B8AA);
static_assert(ctsha::detail::sha512_t_initialization_vector<256>.at(7) == 0x0EB72DDC81C52CA2);

// Test SHA functions on a simple string. These can be easily tested manually on the command-line using the shasum
// program:
//   printf "abc" | shasum --algorithm 1
//   printf "abc" | shasum --algorithm 224
//   printf "abc" | shasum --algorithm 256
//   printf "abc" | shasum --algorithm 384
//   printf "abc" | shasum --algorithm 512
//   printf "abc" | shasum --algorithm 512224
//   printf "abc" | shasum --algorithm 512256
static_assert("abc"_sha1       == "a9993e364706816aba3e25717850c26c9cd0d89d"_hex_bytes);
static_assert("abc"_sha224     == "23097d223405d8228642a477bda255b32aadbce4bda0b3f7e36c9da7"_hex_bytes);
static_assert("abc"_sha256     == "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad"_hex_bytes);
static_assert("abc"_sha384     == "cb00753f45a35e8bb5a03d699ac65007272c32ab0eded1631a8b605a43ff5bed8086072ba1e7cc2358ba"
                                  "eca134c825a7"_hex_bytes);
static_assert("abc"_sha512     == "ddaf35a193617abacc417349ae20413112e6fa4e89a97ea20a9eeee64b55d39a2192992a274fc1a836ba"
                                  "3c23a3feebbd454d4423643ce80e2a9ac94fa54ca49f"_hex_bytes);
static_assert("abc"_sha512_224 == "4634270f707b6a54daae7530460842e20e37ed265ceee9a43e8924aa"_hex_bytes);
static_assert("abc"_sha512_256 == "53048e2681941ef99b2e29b76b4c7dabe4c2d0c634fc6d46e0e2f13107e7af23"_hex_bytes);
