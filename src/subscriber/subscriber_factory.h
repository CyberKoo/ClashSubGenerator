//
// Created by Kotarou on 2020/6/26.
//

#ifndef CLASHSUBGENERATOR_SUBSCRIBER_FACTORY_H
#define CLASHSUBGENERATOR_SUBSCRIBER_FACTORY_H

#include <memory>
#include "subscriber.h"
#include "../config.h"

class SubscriberFactory {
public:
    static std::unique_ptr<Subscriber> make(SubscribeType type);
};


#endif //CLASHSUBGENERATOR_SUBSCRIBER_FACTORY_H
