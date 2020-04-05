//
// Created by Kotarou on 2020/3/15.
//
#include <string>
#include <algorithm>
#include <fmt/format.h>

#include "uri.h"
#include "utils.h"

Uri Uri::Parse(const std::string &uri) {
    Uri result;

    typedef std::string::const_iterator iterator_t;

    if (uri.length() == 0)
        return result;

    iterator_t uriEnd = uri.end();

    // get query start
    iterator_t queryStart = std::find(uri.begin(), uriEnd, '?');

    // protocol
    iterator_t protocolStart = uri.begin();
    iterator_t protocolEnd = std::find(protocolStart, uriEnd, ':');

    if (protocolEnd != uriEnd) {
        std::string prot = &*(protocolEnd);
        if ((prot.length() > 3) && (prot.substr(0, 3) == "://")) {
            result.Protocol = Utils::str_tolower(std::string(protocolStart, protocolEnd));
            protocolEnd += 3;   //      ://
        } else
            protocolEnd = uri.begin();  // no protocol
    } else {
        protocolEnd = uri.begin();  // no protocol
    }

    if (result.Protocol.empty()) {
        throw InvalidURIException(fmt::format("URI doesn't have a valid schema, {0}", uri));
    }

    // host
    iterator_t hostStart = protocolEnd;
    iterator_t pathStart = std::find(hostStart, uriEnd, '/');
    iterator_t hostEnd = std::find(protocolEnd, (pathStart != uriEnd) ? pathStart : queryStart, ':');

    result.Host = std::string(hostStart, hostEnd);

    // port
    if ((hostEnd != uriEnd) && ((&*(hostEnd))[0] == ':')) {
        hostEnd++;
        iterator_t portEnd = (pathStart != uriEnd) ? pathStart : queryStart;
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
    if (pathStart != uriEnd) {
        result.Path = std::string(pathStart, queryStart);
    }

    // query
    if (queryStart != uriEnd) {
        result.QueryString = std::string(queryStart, uri.end());
    }

    return result;
}

const std::string &Uri::getQueryString() const {
    return QueryString;
}

const std::string &Uri::getPath() const {
    return Path;
}

const std::string &Uri::getProtocol() const {
    return Protocol;
}

const std::string &Uri::getHost() const {
    return Host;
}

int Uri::getPort() const {
    return Port;
}
