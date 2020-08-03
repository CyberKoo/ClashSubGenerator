//
// Created by Kotarou on 2020/8/3.
//

#ifndef CLASHSUBGENERATOR_PROXY_DECODER_FACTORY_H
#define CLASHSUBGENERATOR_PROXY_DECODER_FACTORY_H


#include <memory>

class ProxyDecoder;

class ProxyDecoderFactory {
public:
    static std::unique_ptr<ProxyDecoder> make(std::string_view protocol);
};


#endif //CLASHSUBGENERATOR_PROXY_DECODER_FACTORY_H
