//
// Created by Kotarou on 2020/3/15.
//
#include <spdlog/spdlog.h>
#include <httplib.h>
#include <version.h>

#include "exception/invalid_http_status_exception.h"
#include "exception/request_failure_exception.h"
#include "uri.h"
#include "httpclient.h"
#include "filesystem.h"

std::unique_ptr<httplib::Client> HttpClient::connect(const Uri &uri) {
    auto client = std::make_unique<httplib::Client>(
            fmt::format("{}://{}:{}", uri.getSchema(), uri.getHost(), uri.getPort()).data());

    // if is https
    if (uri.getSchema() == "https") {
        auto ca_path = get_ca_path();

        if (!ca_path.empty()) {
            client->set_ca_cert_path(ca_path.c_str());
            client->enable_server_certificate_verification(true);
        }
    }

    client->set_default_headers({{"User-Agent", get_user_agent()}});
    client->set_connection_timeout(5);
    client->set_read_timeout(5, 1000);
    client->set_follow_location(true);

    return client;
}

std::string HttpClient::get(const Uri &uri) {
    auto client = HttpClient::connect(uri);
    auto response = client->Get(fmt::format("{}?{}", uri.getPath(), uri.getQueryString()).c_str());

    if (response) {
        if (response->status == 200) {
            return response->body;
        } else {
            throw InvalidHttpStatusException(fmt::format("Server responded with status {0}", response->status));
        }
    } else {
        throw RequestFailureException(fmt::format("Fetch {0} failed", uri.getRawUri()));
    }
}

std::string HttpClient::get(std::string_view uri) {
    spdlog::debug("Fetch uri {}", uri);
    auto parse_result = Uri::Parse(uri);

    return get(parse_result);
}

std::string HttpClient::get_ca_path() {
    static std::string path;
    static bool inited = false;
    constexpr std::string_view search_path[]{
            "/etc/ssl/certs/ca-certificates.crt",                // Debian/Ubuntu/Gentoo etc.
            "/etc/pki/tls/certs/ca-bundle.crt",                  // Fedora/RHEL 6
            "/etc/ssl/ca-bundle.pem",                            // OpenSUSE
            "/etc/pki/tls/cacert.pem",                           // OpenELEC
            "/etc/pki/ca-trust/extracted/pem/tls-ca-bundle.pem", // CentOS/RHEL 7
            "/usr/local/etc/openssl/cert.pem",                   // MacOS via Homebrew
            "./ca.pem"                                           // Load local
    };

    // search
    if (path.empty() && !inited) {
        inited = true;
        spdlog::trace("Start initialize httpclient");
        spdlog::trace("Looking for CA bundle");
        for (const auto &spath : search_path) {
            if (FileSystem::exists(spath)) {
                spdlog::debug("Found CA bundle at {}", spath);
                spdlog::debug("Server certificate verification enabled");
                path = spath;
                break;
            }
        }
    }

    return path;
}

std::string HttpClient::get_user_agent() {
    return get_version();
}
