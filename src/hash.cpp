//
// Created by Kotarou on 2020/4/7.
//

#include <iomanip>

#include <openssl/sha.h>
#include <openssl/md5.h>

#include "hash.h"

template<class T>
std::string get_str(const T &hash) {
    std::stringstream ss;

    for (const auto &value : hash) {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int) value;
    }

    return ss.str();
}

std::string Hash::md5(const std::string &str) {
    unsigned char hash[MD5_DIGEST_LENGTH];
    MD5_CTX md5;
    MD5_Init(&md5);
    MD5_Update(&md5, str.c_str(), str.size());
    MD5_Final(hash, &md5);

    return get_str(hash);
}

std::string Hash::sha1(const std::string &str) {
    unsigned char hash[SHA_DIGEST_LENGTH];
    SHA_CTX sha1;
    SHA1_Init(&sha1);
    SHA1_Update(&sha1, str.c_str(), str.size());
    SHA1_Final(hash, &sha1);

    return get_str(hash);
}

std::string Hash::sha256(const std::string &str) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, str.c_str(), str.size());
    SHA256_Final(hash, &sha256);

    return get_str(hash);
}