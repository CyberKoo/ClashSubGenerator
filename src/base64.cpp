//
// Created by Kotarou on 2020/3/19.
//
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <cstring>
#include <memory>
#include <vector>

#include "base64.h"

const auto BIOFreeAll = [](BIO *p) {
    BIO_free_all(p);
};

using bio_ptr = std::unique_ptr<BIO, decltype(BIOFreeAll)>;

std::string padding(const std::string &data) {
    if (data.size() % 3 != 0) {
        auto new_data = data;
        auto padding = 3 - (data.size() % 3);
        for (unsigned long i = 0; i < padding; ++i) {
            new_data += "=";
        }

        return new_data;
    }

    return data;
}

std::string Base64::encode(const std::string &data) {
    auto base64 = bio_ptr(BIO_new(BIO_f_base64()), BIOFreeAll);

    if (data.find('\n') == std::string::npos) {
        BIO_set_flags(base64.get(), BIO_FLAGS_BASE64_NO_NL); //Don't require trailing newlines
    }

    // this will be automatically cleaned by bio_clean_all
    auto sink = BIO_new(BIO_s_mem());

    // attach sink to base64
    BIO_push(base64.get(), sink);

    // write sink
    BIO_write(base64.get(), data.c_str(), data.size());

    BIO_flush(base64.get());

    // extract return value
    const char *encoded;
    const long len = BIO_get_mem_data(sink, &encoded);

    return std::string(encoded, len);
}

Base64::container_type Base64::decode(const std::string &data) {
    auto base64 = bio_ptr(BIO_new(BIO_f_base64()), BIOFreeAll);

    if (data.find('\n') == std::string::npos) {
        BIO_set_flags(base64.get(), BIO_FLAGS_BASE64_NO_NL); //Don't require trailing newlines
    }

    auto input = padding(data);

    // this will be automatically cleaned by bio_clean_all
    auto source = BIO_new_mem_buf(input.c_str(), input.size());

    BIO_push(base64.get(), source);

    // pre calculated max length
    const auto maxlen = input.size() / 4 * 3 + 1;

    // reserve space
    Base64::container_type decoded(maxlen);

    // calculate exact length
    const auto len = BIO_read(base64.get(), decoded.data(), maxlen);

    // resize container
    decoded.resize(len);

    return decoded;
}

std::string Base64::to_string(const Base64::container_type &result) {
    return std::string(result.begin(), result.end());
}