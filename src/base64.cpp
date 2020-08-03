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

std::string padding(std::string_view data) {
    // under normal circumstance this value should never be 1,
    // the reminder value can only be 0, 2 or 3
    unsigned reminder = data.size() % 4;
    auto new_data = std::string(data);
    if (reminder > 1) {

        if (reminder == 2) {
            new_data += "==";
        } else if (reminder == 3) {
            new_data += "=";
        }
    }

    return new_data;
}

std::string Base64::encode(std::string_view data) {
    auto base64 = bio_ptr(BIO_new(BIO_f_base64()), BIOFreeAll);

    if (data.find('\n') == std::string::npos) {
        BIO_set_flags(base64.get(), BIO_FLAGS_BASE64_NO_NL); //Don't require trailing newlines
    }

    // this will be automatically cleaned by bio_clean_all
    auto sink = BIO_new(BIO_s_mem());

    // attach sink to base64
    BIO_push(base64.get(), sink);

    // write sink
    BIO_write(base64.get(), data.data(), data.size());

    BIO_flush(base64.get());

    // extract return value
    const char *encoded;
    const long len = BIO_get_mem_data(sink, &encoded);

    return std::string(encoded, len);
}

std::string Base64::decode(std::string_view data) {
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
    Base64::container_t decoded(maxlen);

    // calculate exact length
    const auto len = BIO_read(base64.get(), decoded.data(), maxlen);

    // resize container
    decoded.resize(len);

    return std::string(decoded.begin(), decoded.end());
}
