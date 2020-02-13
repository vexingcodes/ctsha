/// This is a header-only library allowing compile-time only (consteval) calculation of SHA-1, SHA-224, SHA-256,
/// SHA-384, SHA-512, SHA-512/224, and SHA-512/256 digests as defined in FIPS 180-4. See
/// https://nvlpubs.nist.gov/nistpubs/FIPS/NIST.FIPS.180-4.pdf. All constants are derived from first principles where
/// possible, rather than just hardcoding the constants given by the document, so one can see where the magic numbers
/// come from. While the algorithm appears to give the right answers, I offer no guarantee that there are no bugs. Use
/// this library at your own risk.

#pragma once

#include <algorithm>
#include <array>
#include <bit>
#include <cstdint>
#include <stdexcept>

// The standard says std::endian supports "corner case" platforms with no or mixed endianness, but we don't.
static_assert(!(std::endian::native == std::endian::little && std::endian::native == std::endian::big));
static_assert(std::endian::native == std::endian::little || std::endian::native == std::endian::big);

namespace ctsha {

/// The detail namespace contains the "private" functions of the library.
namespace detail {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Utilities                                                                                                          //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// Seems silly to have this, but it makes the code more readable than having a magic number everywhere.
constexpr std::size_t bits_per_byte = 8;

/// Determines the number of bits in memory required for the given type. Like sizeof, but for bits instead of bytes.
///
/// @tparam data_t The type whose size in bits is to be calculated.
template <typename data_t>
constexpr std::size_t bits = sizeof(data_t) * bits_per_byte;

/// Determines the number of bytes required to store the given number of bits. Rounds up to the nearest byte.
///
/// @tparam bits The number of bits whose size in bytes is to be calculated.
template <std::size_t bits>
constexpr std::size_t bytes = (bits + (bits_per_byte - 1)) / bits_per_byte;

/// Perform byte-swapping on some unsigned integral type at compile time.
///
/// @tparam data_t The data type whose bytes are being swapped.
///
/// @param value The value whose byte-swapped representation is being computed.
///
/// @returns A data_t containing the byte-swapped value.
template <typename data_t> requires std::unsigned_integral<data_t>
consteval data_t byte_swap(data_t value) {
    data_t swapped{};
    for (std::size_t byte_index = 0; byte_index < sizeof(data_t); ++byte_index) {
        data_t byte_value = (value & (data_t{0xff} << byte_index * bits_per_byte)) >> (byte_index * bits_per_byte);
        swapped |= byte_value << ((sizeof(data_t) - byte_index - 1) * bits_per_byte);
    }
    return swapped;
}

/// Takes a value in big endian byte order and converts it to host byte order at compile time.
///
/// @tparam data_t The data type whose bytes are being swapped.
///
/// @param value The big endian byte order value to convert to host byte order.
///
/// @returns A data_t containing the big endian byte order representation of value.
template <typename data_t>
consteval data_t big_endian_to_host(data_t value) {
    return std::endian::native == std::endian::little ? byte_swap(value) : value;
}

/// Converts an array of integers to an array of bytes with a specified endianness.
///
/// @tparam endianness   The desired byte order.
/// @tparam data_t       The type of integers in the array.
/// @tparam num_elements The number of integerts in the array.
///
/// @param value The array of integers to convert to bytes.
///
/// @returns The converted array.
template <std::endian endianness, typename data_t, std::size_t num_elements> requires std::unsigned_integral<data_t>
consteval std::array<std::byte, sizeof(data_t) * num_elements> to_bytes(const std::array<data_t, num_elements>& value) {
    std::array<std::byte, sizeof(data_t) * num_elements> result{};
    for (auto current_byte = result.begin(); const data_t& element : value) {
        for (std::size_t byte_index = 0; byte_index < sizeof(data_t); ++byte_index, ++current_byte) {
            std::size_t shift = endianness == std::endian::little ? byte_index * bits_per_byte
                                                                  : (sizeof(data_t) - byte_index - 1) * bits_per_byte;
            *current_byte = static_cast<std::byte>((element & (data_t{0xff} << shift)) >> shift);
        }
    }

    return result;
}

/// Creates an array where each element is generated using a function that takes its position in the array as a template
/// argument.
///
/// @tparam num_elements The number of elements in the array.
/// @tparam func_t       The type of predicate function used to generate the elements.
///
/// @param func The function used to generate the elements.
///
/// @returns An array of data_t containing num_elements where each element is generated by func.
template <std::size_t num_elements, typename func_t>
consteval auto generate_array(func_t func) -> std::array<decltype(func.template operator()<0>()), num_elements>
{
    return [&func]<std::size_t... indices>(std::index_sequence<indices...>) consteval {
        return std::array{func.template operator()<indices>()...};
    }(std::make_index_sequence<num_elements>{});
}

/// Compute the nth root of a number using the Newton-Raphson method on 128-bit floating-point numbers.
///
/// @param value The number whose root is to be computed.
/// @param root  Which root to calculate. (2 for square root, 3 for cube root, etc.)
/// @param guess The current approximate guess.
///
/// @returns The nth root of the provided number.
///
/// @note See https://en.wikipedia.org/wiki/Newton%27s_method.
///
/// @note It would be great to not need quad-precision floating point accuracy, but unfortunately we do. A long double
///       does not have sufficient precision to calculate the 64-bit constants. This is pretty much the only
///       non-standard area of this library. We could just hardcode all of the constants to avoid non-standard code, but
///       a goal of this library is to derive all constants using code wherever possible, so we use this nonstandard
///       extension.
///
/// @note Unfortunately using FLT128_EPSILON instead of 1.0e-32 causes us to infinitely recurse for some numbers. Using
///       1.0e-32 gives us enough precision for this application though, and does not fail.
consteval __float128 float128_root(__float128 value, std::size_t root, __float128 guess = __float128{1.0}) {
    __float128 exp = guess;
    for (std::size_t i = 0; i < root - 2; ++i, exp *= guess);
    __float128 guess2 = ((static_cast<__float128>(root) - __float128{1.0}) * guess + value / exp) / root;
    return (guess - guess2 <= __float128{1.0e-32} && guess - guess2 >= __float128{-1.0e-32})
           ? guess : float128_root(value, root, guess2);
}

/// Determines whether or not a given number is prime using a recursive implementation of the Sieve of Eratosthenes.
/// (Well, it's not quite the Sieve of Eratoshenese, since we check multiples of _every_ number instead of just checking
/// the primes, but it works on the same principles.)
///
/// @param value   The number to test for primality.
/// @param divisor The current divisor checking if value is divisible by divisor and hence _not_ prime.
///
/// @returns True if the given number is prime, false otherwise.
///
/// @note This is a very inefficient implementation, and should only be used for small numbers.
///
/// @note This does NOT give correct results for i==0 and i==1. This function is meant only to be used by next_prime.
consteval bool is_prime(std::uint64_t value, std::uint64_t divisor = 2) {
    return (divisor * divisor > value) ? true : (value % divisor == 0) ? false : is_prime(value, divisor + 1);
}

/// Calculates the first prime number at or after value.
///
/// @param value The starting point to search for the next prime number.
///
/// @returns The value of the first prime number at or after the passed in value.
///
/// @note This is a very inefficient implementation, and should only be used for small numbers.
consteval std::uint64_t next_prime(std::uint64_t value) {
    return is_prime(value, 2) ? value : next_prime(value + 1);
}

/// Calculates the nth prime number, zero indexed.
///
/// @param index The zero-based index of the prime number to generate.
/// @param value The value of the current test. Do not manually set this parameter.
///
/// @returns The value index-th prime number, zero indexed.
///
/// @note This is a very inefficient implementation, and should only be used for small numbers.
consteval std::uint64_t prime(std::uint64_t index, std::uint64_t value = 2) {
    return (index == 0) ? value : prime(index - 1, next_prime(value + 1));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Generic SHA Functions                                                                                              //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// Ensures a type is a valid type for a SHA hash word. SHA hashes use either 32-bit or 64-bit words.
template <typename data_t>
concept sha_word = std::is_same_v<data_t, std::uint32_t> || std::is_same_v<data_t, std::uint64_t>;

/// In all SHA algorithms a message is broken down into blocks of 16 words.
template <typename word_t> requires sha_word<word_t>
using block_t = std::array<word_t, 16>;

/// The ROTR(x) function defined in FIPS 180-4 section 3.2 (Operations on Words) bullet point 4.
///
/// @tparam num_bits The number of bits to rotate.
/// @tparam word_t   The type of number to rotate.
///
/// @param x The number to be rotated right.
///
/// @returns The result of right-rotating the bits of x by num_bits.
///
/// @note We define our own function instead of using std::rotr so we can define additional constraints and checks.
template <std::size_t num_bits, typename word_t> requires (sha_word<word_t> && num_bits < bits<word_t>)
consteval word_t rotate_right(word_t x) {
    return (x >> num_bits) | (x << (bits<word_t> - num_bits));
}

/// The ROTL(x) function defined in FIPS 180-4 section 3.2 (Operations on Words) bullet point 5.
///
/// @tparam num_bits The number of bits to rotate.
/// @tparam word_t   The type of number to rotate.
///
/// @param x The number to be rotated left.
///
/// @returns The result of left-rotating the bits of x by num_bits.
///
/// @note We define our own function instead of using std::rotl so we can define additional constraints and checks.
template <std::size_t num_bits, typename word_t> requires (sha_word<word_t> && num_bits < bits<word_t>)
consteval word_t rotate_left(word_t x) {
    return (x << num_bits) | (x >> (bits<word_t> - num_bits));
}

/// The "Ch(x, y, z)" function defined in FIPS 180-4 sections 4.1.1, 4.1.2, and 4.1.3.
///
/// For each bit i in words x, y, and z if x[i] is set then result[i] is y[i], otherwise result[i] is z[i]. In other
/// words the bit in x "chooses" if the result bit comes from y or z.
///
/// @tparam word_t The type of number being used.
template <typename word_t> requires sha_word<word_t>
constexpr word_t choose(word_t x, word_t y, word_t z) {
    return (x & y) ^ (~x & z);
}

/// The "Maj(x, y z)" function defined in FIPS 180-4 sections 4.1.1, 4.1.2, and 4.1.3.
///
/// For each bit i in words x, y, and z if the majority of x[i], y[i], and z[i] are set then result[i] is set, otherwise
/// result[i] is not set.
///
/// @tparam word_t The type of number being used.
template <typename word_t> requires sha_word<word_t>
constexpr word_t majority(word_t x, word_t y, word_t z) {
    return (x & y) ^ (x & z) ^ (y & z);
}

/// The "Parity(x, y, z)" function defined in FIPS 180-4 section 4.1.1.
///
/// For each bit i in words x, y, and z if there are an even number of set bits in x[i], y[i], z[i] then result[i] is
/// not set, otherwise result[i] is set.
///
/// @tparam word_t The type of number being used.
///
/// @note This is only used in SHA-1, so std::uint32_t is used here instead of a templated word_t.
constexpr std::uint32_t parity(std::uint32_t x, std::uint32_t y, std::uint32_t z) {
    return x ^ y ^ z;
}

/// The Σ0 function defined in FIPS 180-4 section 4.1.2 equation 4.4.
consteval std::uint32_t Σ0(std::uint32_t x) {
    return rotate_right<2>(x) ^ rotate_right<13>(x) ^ rotate_right<22>(x);
}

/// The Σ0 function defined in FIPS 180-4 section 4.1.3 equation 4.10.
consteval std::uint64_t Σ0(std::uint64_t x) {
    return rotate_right<28>(x) ^ rotate_right<34>(x) ^ rotate_right<39>(x);
}

/// The Σ1 function defined in FIPS 180-4 section 4.1.2 equation 4.5.
consteval std::uint32_t Σ1(std::uint32_t x) {
    return rotate_right<6>(x) ^ rotate_right<11>(x) ^ rotate_right<25>(x);
}

/// The Σ1 function defined in FIPS 180-4 section 4.1.3 equation 4.11.
consteval std::uint64_t Σ1(std::uint64_t x) {
    return rotate_right<14>(x) ^ rotate_right<18>(x) ^ rotate_right<41>(x);
}

/// The σ0 function defined in FIPS 180-4 section 4.1.2 equation 4.6.
consteval std::uint32_t σ0(std::uint32_t x) {
    return rotate_right<7>(x) ^ rotate_right<18>(x) ^ (x >> 3);
}

/// The σ0 function defined in FIPS 180-4 section 4.1.3 equation 4.12.
consteval std::uint64_t σ0(std::uint64_t x) {
    return rotate_right<1>(x) ^ rotate_right<8>(x) ^ (x >> 7);
}

/// The σ1 function defined in FIPS 180-4 section 4.1.2 equation 4.7.
consteval std::uint32_t σ1(std::uint32_t x) {
    return rotate_right<17>(x) ^ rotate_right<19>(x) ^ (x >> 10);
}

/// The σ1 function defined in FIPS 180-4 section 4.1.3 equation 4.13.
consteval std::uint64_t σ1(std::uint64_t x) {
    return rotate_right<19>(x) ^ rotate_right<61>(x) ^ (x >> 6);
}

/// Performs message preprocessing as described by FIPS 180-4 section 5.
///
/// @tparam word_t    The type of word used by the SHA algorithm.
/// @tparam num_bytes The number of bytes in the original message whose hash is being calculated.
///
/// @param original_message The message to preprocess.
///
/// @returns An array of blocks representing the preprocessed message.
///
/// @note The returned message is in big endian byte order.
///
/// @note The SHA algorithms support computing hashes on messages that are not an exact number of bytes, but this
///       function requires the message to be an exact number of bytes.
template <typename word_t, std::size_t original_bytes> requires sha_word<word_t>
consteval auto preprocess_message(const std::array<std::byte, original_bytes>& original_message) {
    // The message will have a '1' bit appended, and then a two-word length.
    constexpr std::size_t min_bits = (original_bytes * bits_per_byte) + 1 + (2 * bits<word_t>);

    // The message must be an exact number of blocks, so we pad up to the nearest block boundary.
    constexpr std::size_t blocks = (min_bits + (bits<block_t<word_t>> - 1)) / bits<block_t<word_t>>;

    // Copy the original message into a buffer with the correct amount of padding.
    std::array<std::byte, blocks * sizeof(block_t<word_t>)> message{};
    std::copy(original_message.begin(), original_message.end(), message.begin());

    // Append the '1' bit to the end of the message.
    *(message.begin() + original_message.size()) = std::byte{0b10000000};

    // Copy eight size bites into the end of the message. SHA-384, SHA-512, and SHA-512/t actually use a 128-bit size,
    // but we restrict ourselves to 64 bits, which is plenty for any realistic message length.
    auto size = to_bytes<std::endian::big>(std::array{static_cast<std::uint64_t>(original_bytes) * bits_per_byte});
    std::copy(size.begin(), size.end(), message.begin() + (message.size() - size.size()));

    // Convert the message bytes to blocks of words.
    std::array<block_t<word_t>, blocks> message_blocks{};
    auto bytes_iter = message.begin();
    for (block_t<word_t>& block : message_blocks)
        for (word_t& word : block)
            for (std::size_t byte_index = 0; byte_index < sizeof(word_t); ++byte_index, ++bytes_iter)
                word |= static_cast<word_t>(*bytes_iter) << (byte_index * bits_per_byte);

    return message_blocks;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Constants                                                                                                          //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// An array of function pointers representing the eighty SHA-1 functions as defined in FIPS 180-4 section 4.1.1.
constexpr auto sha1_functions = generate_array<80>([]<std::size_t index>() consteval {
    constexpr std::array funcs{choose<std::uint32_t>, parity, majority<std::uint32_t>, parity};
    return funcs.at(index / 20);
});

/// An array of integers containing the eighty SHA-1 constants as defined in FIPS 180-4 section 4.2.1.
///
/// The FIPS document does not discuss how these are calculated, but they appear to be the most significant 32 bits of
/// the square roots of some numbers (including both the integer and fractional parts).
///
///      0 <= t <= 19  -->  sqrt(2)
///     20 <= t <= 39  -->  sqrt(3)
///     40 <= t <= 59  -->  sqrt(5)
///     60 <= t <= 79  -->  sqrt(10)
constexpr auto sha1_constants = generate_array<80>([]<std::size_t index>() consteval {
    constexpr std::array<std::uint32_t, 4> roots{2, 3, 5, 10};
    return static_cast<uint32_t>(float128_root(roots.at(index / 20), 2) * (1 << 30));
});

/// An array of integers containing the five SHA-1 initialization vector values as defined in FIPS 180-4 section 5.3.1.
///
/// They are presented in big-endian format here and byte-swapped so the pattern is more obvious. The first two entries
/// are simply the hex digits 0 through f ascending. The next two entries are hex digits f through 0 descending. The
/// last entry is the digits f through c descending in the most significant nibble of each byte, and the digits 0
/// through 3 ascending in the least significant nibble of each byte.
constexpr std::array<std::uint32_t, 5> sha1_initialization_vector = {
    byte_swap(std::uint32_t{0x01234567}),
    byte_swap(std::uint32_t{0x89abcdef}),
    byte_swap(std::uint32_t{0xfedcba98}),
    byte_swap(std::uint32_t{0x76543210}),
    byte_swap(std::uint32_t{0xf0e1d2c3})
};

/// Calculates a constant value used by one of the SHA-2 functions. These values are always the first w bits of the
/// fractional parts of the nth root of a prime number p where w is the number of bits in word_t.
///
/// @tparam word_t The type of number being used.
///
/// @param prime_number The prime number used to calculate the constant.
/// @param root         Which root to compute. (2 for square root, 3 for cube root, etc.)
///
/// @returns The first w bits of the frational part of the nth root of p.
template <typename word_t> requires sha_word<word_t>
consteval word_t sha2_constant(word_t prime_number, std::size_t root) {
    __float128 value          = float128_root(prime_number, root);
    __float128 fractional     = value - static_cast<__int128_t>(value);
    __float128 big_fractional = fractional * (static_cast<__int128_t>(1) << bits<word_t>);
    return static_cast<word_t>(big_fractional);
}

/// An array of the 64 32-bit constants used by the SHA-224 and SHA-256 algorithms defined in FIPS 180-4 section 4.2.2:
///
///     "These words represent the first thirty-two bits of the fractional parts of the cube roots of the first
///     sixty-four prime numbers."
constexpr auto sha2_32_bit_constants = generate_array<64>([]<std::size_t index>() consteval {
        return sha2_constant(static_cast<std::uint32_t>(prime(index)), 3);
});

/// An array of the 80 64-bit constants used by the SHA-384, SHA-512, and SHA-512/t algorithms defined in FIPS 180-4
/// section 4.2.3:
///
///    "These words represent the first sixty-four bits of the fractional parts of the cube roots of the first eighty
///    prime numbers."
constexpr auto sha2_64_bit_constants = generate_array<80>([]<std::size_t index>() consteval {
        return sha2_constant(prime(index), 3);
});

/// An array of the 8 32-bit integers containing the eight SHA-244 initialization vector values as defined in FIPS 180-4
/// section 5.3.2.
///
/// The document does not discuss the derivation of these values, but they are the lower 32 bits of the first 64 bits of
/// the fractional parts of the square roots of the ninth through sixteenth prime numbrers.
constexpr auto sha224_initialization_vector = generate_array<8>([]<std::size_t index>() consteval {
        return static_cast<std::uint32_t>(sha2_constant(static_cast<std::uint64_t>(prime(index + 8)), 2));
});

/// An array of the 8 32-bit integers containing the eight SHA-256 initialization vector values as defined in FIPS 180-4
/// section 5.3.3.
///
///     "These words were obtained by taking the first thirty-two bits of the fractional parts of the square roots of
///     the first eight prime numbers."
constexpr auto sha256_initialization_vector = generate_array<8>([]<std::size_t index>() consteval {
        return sha2_constant(static_cast<std::uint32_t>(prime(index)), 2);
});

/// An array of the 8 64-bit integers containing the eight SHA-384 initialization vector values as defined in FIPS 180-4
/// section 5.3.4.
///
///     "These words were obtained by taking the first sixty-four bits of the fractional parts of the square roots of
///     the ninth through sixteenth prime numbers."
constexpr auto sha384_initialization_vector = generate_array<8>([]<std::size_t index>() consteval {
        return sha2_constant(prime(index + 8), 2);
});

/// An array of the 8 64-bit integers containing the eight SHA-512 initialization vector values as defined in FIPS 180-4
/// section 5.3.5.
///
///     "These words were obtained by taking the first sixty-four bits of the fractional parts of the square roots of
///     the first eight prime numbers."
constexpr auto sha512_initialization_vector = generate_array<8>([]<std::size_t index>() consteval {
        return sha2_constant(prime(index), 2);
});

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Top-Level Hash Functions                                                                                           //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// Computes the SHA-1 hash of a given message.
///
/// @tparam num_bytes The length of the message.
///
/// @param message The message for which the SHA-1 hash is being computed.
///
/// @returns An array of bytes representing the SHA-1 hash result.
template <std::size_t num_bytes>
consteval std::array<std::byte, bytes<160>> sha1(const std::array<std::byte, num_bytes>& message) {
    auto state = sha1_initialization_vector;

    for (const auto& block : preprocess_message<std::uint32_t>(message)) {

        // Prepare the message schedule.
        std::array<std::uint32_t, 80> w{};
        for (std::size_t t = 0; t < w.size(); ++t)
            w.at(t) = (t < 16) ? big_endian_to_host(block.at(t))
                               : rotate_left<1>(w.at(t - 3) ^ w.at(t - 8) ^ w.at(t - 14) ^ w.at(t - 16));

        // Initialize the working variables. (a=0, b=1, c=2, d=3, e=4)
        auto v = state;

        // Compute new values for the working variables.
        for (std::size_t t = 0; t < w.size(); ++t) {
            std::uint32_t upper_t = rotate_left<5>(v.at(0)) + sha1_functions.at(t)(v.at(1), v.at(2), v.at(3)) +
                                    v.at(4) + sha1_constants.at(t) + w.at(t);
            v.at(4) = v.at(3);                  // e = d
            v.at(3) = v.at(2);                  // d = c
            v.at(2) = rotate_left<30>(v.at(1)); // c = ROTL30(b)
            v.at(1) = v.at(0);                  // b = a
            v.at(0) = upper_t;                  // a = T
        }

        // Compute the intermediate hash value.
        for (auto si = state.begin(), vi = v.begin(); si != state.end() && vi != v.end(); ++si, ++vi)
            *si = *vi + *si;
    }

    return to_bytes<std::endian::big>(state);
}

/// Computes the SHA-2 hash of a given message. This function performs the work for SHA-224, SHA-256, SHA-384, and
/// SHA-512.
///
/// @tparam digest_bits   The number of desired bits in the digest.
/// @tparam word_t        The type of words used by the specific SHA-2 algorithm. This parameter is usually deduced.
/// @tparam num_constants The number of constants in the given array of constants. This parameter is usually deduced.
/// @tparam num_bytes     The number of bytes in the original message for which the SHA-2 hash is being computed. This
///                       parameter is usually deduced.
///
/// @param message               The message for which the SHA-2 hash is being calculated.
/// @param initialization_vector The initialization vector to use when computing the hash.
/// @param constants             The set of constants to use when computing the hash.
///
/// @returns An array of bytes representing the SHA-2 hash result.
template <std::size_t digest_bits, typename word_t, std::size_t num_constants, std::size_t num_bytes>
    requires sha_word<word_t>
consteval std::array<std::byte, bytes<digest_bits>> sha2(const std::array<std::byte, num_bytes>&  message,
                                                         const std::array<word_t, 8>&             initialization_vector,
                                                         const std::array<word_t, num_constants>& constants) {
    auto state = initialization_vector;

    for (const auto& block : preprocess_message<word_t>(message)) {

        // Prepare the message schedule.
        std::array<word_t, num_constants> w{};
        for (std::size_t t = 0; t < w.size(); ++t)
            w.at(t) = (t < 16) ? big_endian_to_host(block.at(t))
                               : σ1(w.at(t - 2)) + w.at(t - 7) + σ0(w.at(t - 15)) + w.at(t - 16);

        // Initialize the working variables. (a=0, b=1, c=2, d=3, e=4, f=5, g=6, h=7)
        decltype(state) v = state;

        // Compute new values for the working variables.
        for (std::size_t t = 0; t < w.size(); ++t) {
            word_t t1 = v.at(7) + Σ1(v.at(4)) + choose(v.at(4), v.at(5), v.at(6)) + constants.at(t) + w.at(t);
            word_t t2 = Σ0(v.at(0)) + majority(v.at(0), v.at(1), v.at(2));
            v.at(7) = v.at(6);      // h = g
            v.at(6) = v.at(5);      // g = f
            v.at(5) = v.at(4);      // f = e
            v.at(4) = v.at(3) + t1; // e = d + t1
            v.at(3) = v.at(2);      // d = c
            v.at(2) = v.at(1);      // c = b
            v.at(1) = v.at(0);      // b = a
            v.at(0) = t1 + t2;      // a = t1 + t2
        }

        // Compute the intermediate hash value.
        for (auto si = state.begin(), vi = v.begin(); si != state.end() && vi != v.end(); ++si, ++vi)
            *si = *vi + *si;
    }

    // Truncate the digest if needed. If not then just return the full digest.
    auto full_digest = to_bytes<std::endian::big>(state);
    if constexpr(bytes<digest_bits> < sizeof(state)) {
        std::array<std::byte, bytes<digest_bits>> truncated_digest{};
        std::copy(full_digest.begin(), full_digest.begin() + bytes<digest_bits>, truncated_digest.begin());
        return truncated_digest;
    } else {
        return full_digest;
    }
}

/// Computes the initialization vector for the SHA-512/t hashes. (FIPS 180-4 section 5.3.6.)
///
/// @tparam hash_bits The number of bits in the truncated SHA-512/t hash.
///
/// @note This is separate from the other initialization vectors to avoid needing to declare a prototype for sha2.
template <std::size_t hash_bits> requires (hash_bits != 0 && hash_bits != 384)
constexpr std::array<std::uint64_t, 8> sha512_t_initialization_vector = []() consteval {
    using b = std::byte;

    /// The SHA-512/t initialization vectors rely on an intermediate initialization vector which is the SHA-512
    /// initialization vector with every byte xored with 0xa5. See FIPS 180-4 section 5.3.6.
    constexpr std::array<std::uint64_t, 8> intermediate_iv = []() consteval {
        auto iv = sha512_initialization_vector;
        std::for_each(iv.begin(), iv.end(), [](auto& entry) { entry ^= 0xa5a5a5a5a5a5a5a5; });
        return iv;
    }();

    // Compute the SHA-512 hash of the string "SHA-512/t" where "t" is the ASCII string representation of hash_bits. The
    // output of that will be used as the initialization vector.
    auto hash_message = [&intermediate_iv]<std::size_t num_digits>(const std::array<b, num_digits>& t) consteval {
        std::array<b, 8> prefix { b{'S'}, b{'H'}, b{'A'}, b{'-'}, b{'5'}, b{'1'}, b{'2'}, b{'/'} };
        std::array<b, 8 + num_digits> message;
        std::copy(prefix.begin(), prefix.end(), message.begin());
        std::copy(t.begin(), t.end(), message.begin() + prefix.size());
        return detail::sha2<512>(message, intermediate_iv, sha2_64_bit_constants);
    };

    // Compute the hash. We need a branch for each number of digits that t can be.
    std::array<std::byte, bytes<512>> hash_result{};
    if constexpr (hash_bits < 10) {
        hash_result = hash_message(std::array{b{hash_bits + '0'}});
    } else if constexpr (hash_bits < 100) {
        hash_result = hash_message(std::array{b{hash_bits / 10 + '0' }, b{hash_bits % 10 + '0'}});
    } else {
        hash_result =
            hash_message(std::array{b{hash_bits / 100 + '0'}, b{hash_bits % 100 / 10 + '0' }, b{hash_bits % 10 + '0'}});
    }

    // Convert the hash result (an array of bytes) into an initialization vector (array of host byte order uint64_ts).
    std::array<std::uint64_t, 8> iv{};
    for (std::size_t iv_index = 0; iv_index < iv.size(); ++iv_index) {
        std::uint64_t iv_value{};
        for (std::size_t byte_index = 0; byte_index < bits_per_byte; ++ byte_index) {
            auto index = iv_index * sizeof(std::uint64_t) + byte_index;
            auto shift = byte_index * bits_per_byte;
            iv_value |= static_cast<std::uint64_t>(hash_result.at(index)) << shift;
        }

        iv.at(iv_index) = big_endian_to_host(iv_value);
    }
    return iv;
}();

} // End namespace detail.

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Public Interface                                                                                                   //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// Computes the SHA-1 hash of a byte array.
///
/// @tparam num_bytes The number of bytes in the message for which the SHA-1 hash is being computed.
///
/// @param message The message for which the SHA-1 hash is being computed.
///
/// @returns An array of bytes representing the SHA-1 result.
template <std::size_t num_bytes>
consteval std::array<std::byte, detail::bytes<160>> sha1(const std::array<std::byte, num_bytes>& message) {
    return detail::sha1(message);
}

/// Computes the SHA-224 hash of a byte array.
///
/// @tparam num_bytes The number of bytes in the message for which the SHA-224 hash is being computed.
///
/// @param message The message for which the SHA-224 hash is being computed.
///
/// @returns An array of bytes representing the SHA-224 result.
template <std::size_t num_bytes>
consteval std::array<std::byte, detail::bytes<224>> sha224(const std::array<std::byte, num_bytes>& message) {
    return detail::sha2<224>(message, detail::sha224_initialization_vector, detail::sha2_32_bit_constants);
}

/// Computes the SHA-256 hash of a byte array.
///
/// @tparam num_bytes The number of bytes in the message for which the SHA-256 hash is being computed.
///
/// @param message The message for which the SHA-256 hash is being computed.
///
/// @returns An array of bytes representing the SHA-256 result.
template <std::size_t num_bytes>
consteval std::array<std::byte, detail::bytes<256>> sha256(const std::array<std::byte, num_bytes>& message) {
    return detail::sha2<256>(message, detail::sha256_initialization_vector, detail::sha2_32_bit_constants);
}

/// Computes the SHA-384 hash of a byte array.
///
/// @tparam num_bytes The number of bytes in the message for which the SHA-384 hash is being computed.
///
/// @param message The message for which the SHA-384 hash is being computed.
///
/// @returns An array of bytes representing the SHA-384 result.
template <std::size_t num_bytes>
consteval std::array<std::byte, detail::bytes<384>> sha384(const std::array<std::byte, num_bytes>& message) {
    return detail::sha2<384>(message, detail::sha384_initialization_vector, detail::sha2_64_bit_constants);
}

/// Computes the SHA-512 hash of a byte array.
///
/// @tparam num_bytes The number of bytes in the message for which the SHA-512 hash is being computed.
///
/// @param message The message for which the SHA-512 hash is being computed.
///
/// @returns An array of bytes representing the SHA-512 result.
template <std::size_t num_bytes>
consteval std::array<std::byte, detail::bytes<512>> sha512(const std::array<std::byte, num_bytes>& message) {
    return detail::sha2<512>(message, detail::sha512_initialization_vector, detail::sha2_64_bit_constants);
}

/// Computes the SHA-512/t hash of a byte array. (FIPS 180-4 section 5.3.6.)
///
/// @tparam hash_bits The number of bits in the final, truncated hash.
/// @tparam num_bytes The number of bytes in the message for which the SHA-512/t hash is being computed.
///
/// @param message The message for which the SHA-512/t hash is being computed.
///
/// @returns An array of bytes representing the SHA-512/t result.
template <std::size_t hash_bits, std::size_t num_bytes> requires (hash_bits != 0 && hash_bits != 384 && hash_bits < 512)
consteval std::array<std::byte, detail::bytes<hash_bits>> sha512_t(const std::array<std::byte, num_bytes>& message) {
    return detail::sha2<hash_bits>(message,
                                   detail::sha512_t_initialization_vector<hash_bits>,
                                   detail::sha2_64_bit_constants);
}

/// This namespace contains operators that allow SHA hashes to be constructed from string literals. Unfortunately, they
/// use a non-standard literal type (literals with templated parameter packs). Hopefully a standards-compliant way to do
/// this comes along soon. Use "using namespace ctsha::literals;" to get them into the current namespace, then something
/// like "foobar"_sha1 will be translated into the SHA-1 hash of the string "foobar" (the string does not have a null
/// terminator). Literals for SHA-512/t other than 224 and 256 are not provided, but could easily be created if needed.
namespace literals {

/// Allows the SHA-1 hash of a message to be computed using a string literal.
///
/// @tparam char_t The type of characters in the string literal. Only char is supported here.
/// @tparam chars  The characters in the string literal.
///
/// @returns A byte array representing the SHA-1 hash of the given string literal.
template <typename char_t, char_t... chars>
static constexpr auto operator "" _sha1() {
    return sha1(std::array<std::byte, sizeof...(chars)>{std::byte{chars}...});
}

/// Allows the SHA-224 hash of a message to be computed using a string literal.
///
/// @tparam char_t The type of characters in the string literal. Only char is supported here.
/// @tparam chars  The characters in the string literal.
///
/// @returns A byte array representing the SHA-224 hash of the given string literal.
template <typename char_t, char_t... chars>
static constexpr auto operator "" _sha224() {
    return sha224(std::array<std::byte, sizeof...(chars)>{std::byte{chars}...});
}

/// Allows the SHA-256 hash of a message to be computed using a string literal.
///
/// @tparam char_t The type of characters in the string literal. Only char is supported here.
/// @tparam chars  The characters in the string literal.
///
/// @returns A byte array representing the SHA-256 hash of the given string literal.
template <typename char_t, char_t... chars>
static constexpr auto operator "" _sha256() {
    return sha256(std::array<std::byte, sizeof...(chars)>{std::byte{chars}...});
}

/// Allows the SHA-384 hash of a message to be computed using a string literal.
///
/// @tparam char_t The type of characters in the string literal. Only char is supported here.
/// @tparam chars  The characters in the string literal.
///
/// @returns A byte array representing the SHA-384 hash of the given string literal.
template <typename char_t, char_t... chars>
static constexpr auto operator "" _sha384() {
    return sha384(std::array<std::byte, sizeof...(chars)>{std::byte{chars}...});
}

/// Allows the SHA-512 hash of a message to be computed using a string literal.
///
/// @tparam char_t The type of characters in the string literal. Only char is supported here.
/// @tparam chars  The characters in the string literal.
///
/// @returns A byte array representing the SHA-512 hash of the given string literal.
template <typename char_t, char_t... chars>
static constexpr auto operator "" _sha512() {
    return sha512(std::array<std::byte, sizeof...(chars)>{std::byte{chars}...});
}

/// Allows the SHA-512/224 hash of a message to be computed using a string literal.
///
/// @tparam char_t The type of characters in the string literal. Only char is supported here.
/// @tparam chars  The characters in the string literal.
///
/// @returns A byte array representing the SHA-512/224 hash of the given string literal.
template <typename char_t, char_t... chars>
static constexpr auto operator "" _sha512_224() {
    return sha512_t<224>(std::array<std::byte, sizeof...(chars)>{std::byte{chars}...});
}

/// Allows the SHA-512/256 hash of a message to be computed using a string literal.
///
/// @tparam char_t The type of characters in the string literal. Only char is supported here.
/// @tparam chars  The characters in the string literal.
///
/// @returns A byte array representing the SHA-512/256 hash of the given string literal.
template <typename char_t, char_t... chars>
static constexpr auto operator "" _sha512_256() {
    return sha512_t<256>(std::array<std::byte, sizeof...(chars)>{std::byte{chars}...});
}

} // End namespace literals.

} // End namespace ctsha.
