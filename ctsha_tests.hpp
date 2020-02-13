/// Utility functions used only by the ctsha tests.
#pragma once

#include <array>
#include <cstddef>
#include <stdexcept>

/// Allows easier, more readable declaration of std::array<std::byte> using a string literal where the string literal is
/// interpreted as hex digits.
///
/// @tparam char_t The type of characters in the string literal. Only char is supported here.
/// @tparam chars  The characters in the string literal.
///
/// @returns A byte array representing the provided hexadecmal string.
///
/// @throws std::invalid_argument if the input string is malformed.
template <typename char_t, char_t... chars> requires (sizeof...(chars) >= 2 && sizeof...(chars) % 2 == 0)
static constexpr std::array<std::byte, sizeof...(chars) / 2> operator "" _hex_bytes() {
    constexpr auto hex2val = [](char c) constexpr {
        if (c >= '0' && c <= '9')
            return c - '0';
        else if (c >= 'a' && c <= 'f')
            return c - 'a' + static_cast<char>(0xa);
        else if (c >= 'A' && c <= 'F')
            return c - 'A' + static_cast<char>(0xa);
        else
            throw std::invalid_argument("Character is not a hex digit.");
    };

    // Convert the characters pairwise into bytes.
    const std::array<char, sizeof...(chars)> char_array{chars...};
    std::array<std::byte, sizeof...(chars) / 2> bytes{};
    for (std::size_t i = 0; i < char_array.size(); i += 2)
        bytes.at(i / 2) = static_cast<std::byte>(hex2val(char_array.at(i)) << 4 | hex2val(char_array.at(i + 1)));
    return bytes;
}

/// Makes a byte array from a string literal.
///
/// @tparam char_t The type of characters in the string literal. Only char is supported here.
/// @tparam chars  The characters in the string literal.
///
/// @returns A byte array representing the provided string.
template <typename char_t, char_t... chars>
static constexpr std::array<std::byte, sizeof...(chars)> operator "" _bytes() {
    return std::array<std::byte, sizeof...(chars)>{std::byte{chars}...};
}
