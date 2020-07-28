//
// Created by Kotarou on 2020/7/9.
//

#ifndef CLASHSUBGENERATOR_ENUM_H
#define CLASHSUBGENERATOR_ENUM_H

enum class Generator {
    CONFIG, PROVIDER
};

enum class SubscribeType {
    CLASH, OTHER
};

enum class ProviderType {
    FILE, HTTP
};

enum class ProxyGroupType {
    SELECT, RELAY, URL_TEST, FALLBACK, LOAD_BALANCE
};

#endif //CLASHSUBGENERATOR_ENUM_H
