//
// Created by Kotarou on 2020/3/19.
//

#ifndef CLASHSUBGENERATOR_V2RAY_SUBSCRIBER_H
#define CLASHSUBGENERATOR_V2RAY_SUBSCRIBER_H

#include "subscriber.h"

class V2raySubscriber : public Subscriber {
public:
    void load(const std::string &uri) override;

private:
    static std::vector<std::string> decode_config(const std::string &config);

    static YAML::Node generate_proxy(const std::string &config);
};


#endif //CLASHSUBGENERATOR_V2RAY_SUBSCRIBER_H
