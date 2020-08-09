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

    // schema
    auto schemaStart = uri.begin();
    auto schemaEnd = std::find(schemaStart, uri.end(), ':');

    if (schemaEnd != uri.end()) {
        std::string port = &*(schemaEnd);
        if ((port.length() > 3) && (port.substr(0, 3) == "://")) {
            result.Schema = Utils::str_tolower(std::string(schemaStart, schemaEnd));
            schemaEnd += 3;   //      ://
        } else {
            schemaEnd = uri.begin();  // no schema
        }
    } else {
        schemaEnd = uri.begin();  // no schema
    }

    if (result.Schema.empty()) {
        throw InvalidURIException(fmt::format("URI doesn't have a valid schema, {0}", uri));
    }

    result.Body = std::string(schemaEnd, uri.end());

    // host
    auto hostStart = schemaEnd;
    auto pathStart = std::find(hostStart, uri.end(), '/');
    auto hostEnd = std::find(schemaEnd, (pathStart != uri.end()) ? pathStart : queryStart, ':');

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
        if (result.Schema == "http") {
            result.Port = 80;
        } else if (result.Schema == "https") {
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

std::string_view Uri::getSchema() const {
    return Schema;
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
