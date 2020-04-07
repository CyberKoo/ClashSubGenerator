//
// Created by Kotarou on 2020/3/19.
//

#ifndef CLASHSUBGENERATOR_SHADOWSOCKS_SUBSCRIBER_H
#define CLASHSUBGENERATOR_SHADOWSOCKS_SUBSCRIBER_H

#include "subscriber.h"

class ShadowsocksSubscriber : public Subscriber {
public:
    void load(const std::string &uri) override;
};


#endif //CLASHSUBGENERATOR_SS_SUBSCRIBER_H
