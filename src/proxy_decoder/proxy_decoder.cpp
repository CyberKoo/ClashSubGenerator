//
// Created by Kotarou on 2020/6/26.
//

#include <algorithm>
#include <yaml-cpp/yaml.h>

#include "proxy_decoder.h"
#include "shadowsocksr_decoder.h"
#include "../uri.h"
#include "../base64.h"

std::string ProxyDecoder::decode_base64(std::string_view data) {
    return Base64::decode(data);
}

std::pair<std::string, std::string> ProxyDecoder::strip_name(std::string_view content) {
    auto comment_start = content.rfind('#');
    if (comment_start != std::string::npos) {
        return std::make_pair(content.substr(comment_start + 1, content.size() - 1).data(),
                              std::string(content.substr(0, comment_start)));
    }

    return std::make_pair("", content.data());
}


