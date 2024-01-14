//
// Created by Kotarou on 2020/4/7.
//

#include <iomanip>
#include <sstream>
#include <openssl/evp.h>

#include "hash.h"


template<class T>
std::string hex2str(const T &hash, const size_t length) {
    std::stringstream ss;

    for (unsigned int i = 0; i < length; ++i) {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);
    }

    return ss.str();
}

std::string getMessageDigest(std::string_view msg, const EVP_MD *algorithm) {
    auto context = std::unique_ptr<EVP_MD_CTX, decltype(&EVP_MD_CTX_free)>(EVP_MD_CTX_new(), EVP_MD_CTX_free);
    if (context) {
        if (EVP_DigestInit_ex(context.get(), algorithm, nullptr)) {
            if (EVP_DigestUpdate(context.get(), msg.data(), msg.size())) {
                unsigned char hash[EVP_MAX_MD_SIZE]{};
                unsigned int length = 0;
                if (EVP_DigestFinal_ex(context.get(), hash, &length)) {
                    return hex2str(hash, length);
                }
            }
        }
    }

    throw std::runtime_error("an error occurred when compute message digest");
}

std::string Hash::md5(std::string_view str) {
    return getMessageDigest(str, EVP_md5());
}

std::string Hash::sha1(std::string_view str) {
    return getMessageDigest(str, EVP_sha1());
}

std::string Hash::sha256(std::string_view str) {
    return getMessageDigest(str, EVP_sha256());
}
