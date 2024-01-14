//
// Created by Kotarou on 2020/3/19.
//
#include <openssl/evp.h>
#include <fmt/format.h>
#include <memory>

#include "base64.h"

#include <stdexcept>

std::string Base64::encode(std::string_view text) {
    int text_len = static_cast<int>(text.length());

    auto len_pred = 4 * ((text_len + 2) / 3);
    auto buffer = std::make_unique<char[]>(len_pred + 1);

    auto len_actual = EVP_EncodeBlock(
        reinterpret_cast<unsigned char *>(buffer.get()),
        reinterpret_cast<const unsigned char *>(text.data()),
        text_len
    );

    if (len_actual == -1) {
        throw std::runtime_error(fmt::format("encode \"{}\" failed", text));
    }

    if (len_pred != len_actual) {
        throw std::runtime_error(fmt::format("Whoops, encoder predicted {}, but we got {}", len_pred, len_actual));
    }

    return {buffer.get()};
}

std::string Base64::decode(std::string_view text) {
    int text_len = static_cast<int>(text.length());

    auto len_pred = 3 * text_len / 4;
    auto buffer = std::make_unique<char[]>(len_pred + 1);

    auto len_actual = EVP_DecodeBlock(
        reinterpret_cast<unsigned char *>(buffer.get()),
        reinterpret_cast<const unsigned char *>(text.data()),
        text_len
    );

    if (len_actual == -1) {
        throw std::runtime_error(fmt::format("decode \"{}\" failed", text));
    }

    if (len_pred != len_actual) {
        throw std::runtime_error(fmt::format("Whoops, decoder predicted {}, but we got {}", len_pred, len_actual));
    }

    return {buffer.get()};
}
