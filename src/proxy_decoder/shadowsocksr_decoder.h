//
// Created by Kotarou on 2020/6/26.
//

#ifndef CLASHSUBGENERATOR_SHADOWSOCKSR_DECODER_H
#define CLASHSUBGENERATOR_SHADOWSOCKSR_DECODER_H

#include <map>
#include "proxy_decoder.h"

class ShadowsocksRDecoder : public ProxyDecoder {
public:
    ShadowsocksRDecoder() = default;

    ~ShadowsocksRDecoder() override = default;

    YAML::Node decode_config(const Uri &uri) override;

protected:
    static std::string decode_base64(std::string_view content);

    static std::map<std::string, std::string> get_ssr_parameters(std::string &query_string);
};


#endif //CLASHSUBGENERATOR_SHADOWSOCKSR_DECODER_H
