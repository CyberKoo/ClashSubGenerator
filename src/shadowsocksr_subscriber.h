//
// Created by Kotarou on 2020/4/5.
//

#ifndef CLASHSUBGENERATOR_SHADOWSOCKSR_SUBSCRIBER_H
#define CLASHSUBGENERATOR_SHADOWSOCKSR_SUBSCRIBER_H

#include "subscriber.h"

class ShadowsocksRSubscriber : public Subscriber {
public:
    void load(const std::string &uri) override;

private:
    static std::vector<std::string> decode_config(const std::string &config);

    static std::map<std::string, std::string> get_parameters(std::string &obfs);

    static YAML::Node transform_config(const std::vector<std::string> &config);
};


#endif //CLASHSUBGENERATOR_SS_SUBSCRIBER_H
