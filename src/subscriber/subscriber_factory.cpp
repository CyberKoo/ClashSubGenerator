//
// Created by Kotarou on 2020/6/26.
//

#include <fmt/format.h>

#include "subscriber_factory.h"
#include "clash_subscriber.h"
#include "other_subscriber.h"
#include "../exception/unsupported_configuration.h"

std::unique_ptr<Subscriber> SubscriberFactory::make(SubscribeType type) {
    switch (type) {
        case SubscribeType::CLASH:
            return std::make_unique<ClashSubscriber>();
        case SubscribeType::OTHER:
            return std::make_unique<OtherSubscriber>();
    }

    throw UnsupportedConfiguration(fmt::format("Unsupported subscribe type {}", type));
}
