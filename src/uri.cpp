//
// Created by Kotarou on 2020/3/15.
//
#include <string>
#include <algorithm>
#include <fmt/format.h>

#include "uri.h"
#include "utils.h"
#include "exception/invalid_uri_exception.h"

Uri Uri::Parse(std::string_view uri) {
    Uri result;

    if (uri.empty()) {
        return result;
    }

    result.RawUri = uri;

    // get query start
    auto queryStart = std::find(uri.begin(), uri.end(), '?');

    // protocol
    auto protocolStart = uri.begin();
    auto protocolEnd = std::find(protocolStart, uri.end(), ':');

    if (protocolEnd != uri.end()) {
        std::string port = &*(protocolEnd);
        if ((port.length() > 3) && (port.substr(0, 3) == "://")) {
            result.Protocol = Utils::str_tolower(std::string(protocolStart, protocolEnd));
            protocolEnd += 3;   //      ://
        } else {
            protocolEnd = uri.begin();  // no protocol
        }
    } else {
        protocolEnd = uri.begin();  // no protocol
    }

    if (result.Protocol.empty()) {
        throw InvalidURIException(fmt::format("URI doesn't have a valid schema, {0}", uri));
    }

    result.Body = std::string(protocolEnd, uri.end());

    // host
    auto hostStart = protocolEnd;
    auto pathStart = std::find(hostStart, uri.end(), '/');
    auto hostEnd = std::find(protocolEnd, (pathStart != uri.end()) ? pathStart : queryStart, ':');

    result.Host = std::string(hostStart, hostEnd);

    // port
    if ((hostEnd != uri.end()) && ((&*(hostEnd))[0] == ':')) {
        hostEnd++;
        auto portEnd = (pathStart != uri.end()) ? pathStart : queryStart;
        auto port = std::string(hostEnd, portEnd);
        if (!port.empty()) {
            result.Port = std::stoi(port);
        }
    }

    if (result.Port == 0) {
        if (result.Protocol == "http") {
            result.Port = 80;
        } else if (result.Protocol == "https") {
            result.Port = 443;
        }
    }

    // path
    if (pathStart != uri.end()) {
        result.Path = std::string(pathStart, queryStart);
    }

    // query
    if (queryStart != uri.end()) {
        result.QueryString = std::string(queryStart + 1, uri.end());
    }

    return result;
}

std::string_view Uri::getQueryString() const {
    return QueryString;
}

std::string_view Uri::getPath() const {
    return Path;
}

std::string_view Uri::getProtocol() const {
    return Protocol;
}

std::string_view Uri::getHost() const {
    return Host;
}

int Uri::getPort() const {
    return Port;
}

std::string_view Uri::getRawUri() const {
    return RawUri;
}

std::string_view Uri::getBody() const {
    return Body;
}
