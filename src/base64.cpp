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

bio_ptr get_base64_ptr() {
    bio_ptr ptr(BIO_new(BIO_f_base64()), BIOFreeAll);
    BIO_set_flags(ptr.get(), BIO_FLAGS_BASE64_NO_NL);

    return ptr;
}

std::string base64::encode(const std::string &data) {
    auto base64 = get_base64_ptr();
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

base64::container_type base64::decode(const std::string &data) {
    auto base64 = get_base64_ptr();
    // this will be automatically cleaned by bio_clean_all
    auto source = BIO_new_mem_buf(data.c_str(), -1);
    BIO_push(base64.get(), source);

    // pre calculated max length
    const auto maxlen = strlen(data.c_str()) / 4 * 3 + 1;

    // reserve space
    base64::container_type decoded(maxlen);

    // calculate exact length
    const auto len = BIO_read(base64.get(), decoded.data(), maxlen);

    // resize container
    decoded.resize(len);

    return decoded;
}
