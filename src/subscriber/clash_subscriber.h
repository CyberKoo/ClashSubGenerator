//
// Created by Kotarou on 2020/3/16.
//

#ifndef CLASHSUBGENERATOR_CLASH_SUBSCRIBER_H
#define CLASHSUBGENERATOR_CLASH_SUBSCRIBER_H

#include "subscriber.h"

class ClashSubscriber : public Subscriber {
public:
    ClashSubscriber() = default;

    void load(const std::string &uri) final;
};

#endif //CLASHSUBGENERATOR_CLASH_SUBSCRIBER_H
