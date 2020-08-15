//
// Created by Kotarou on 2020/3/19.
//

#include <spdlog/spdlog.h>
#include <yaml-cpp/yaml.h>

#include "subscriber.h"
#include "utils.h"
#include "uri.h"
#include "base64.h"
#include "config.h"
#include "config_loader.h"
#include "exception/missing_key_exception.h"
#include "proxy_decoder/proxy_decoder.h"
#include "proxy_decoder/proxy_decoder_factory.h"

Subscriber::Subscriber(SubscribeType type) {
    this->regex_collapse = false;
    this->enable_grouping = false;
    this->exclude_amplified_node = false;
    this->use_emoji = false;
    this->type = type;
}

void Subscriber::load(std::string_view uri) {
    switch (this->type) {
        case SubscribeType::CLASH:
            try {
                clash_config_loader(uri);
            } catch (YAML::ParserException &e) {
                spdlog::critical("Invalid configuration file. {}", e.what());
                std::abort();
            } catch (MissingKeyException &e) {
                spdlog::critical(
                        "{}, the uri supplied does not contain proxy section or is not a valid clash configuration file. You may want to change the subscriber type to other",
                        e.what());
                std::abort();
            }

            return;
        case SubscribeType::OTHER:
            base64_config_loader(uri);
            break;
        case SubscribeType::AUTO:
            try {
                spdlog::debug("Try clash config loader");
                clash_config_loader(uri);
                spdlog::debug("Clash configuration successfully loaded");
            } catch (std::exception &e) {
                spdlog::debug("Fallback to base64 encoded config loader");
                base64_config_loader(uri);
            }
            break;
    }

    // check loaded proxies
    if (proxies.size() > 0) {
        spdlog::info("Total number of proxies loaded {}", proxies.size());
    } else {
        spdlog::warn("No proxy loaded, this is probably not what you expected", proxies.size());
    }
}

void Subscriber::clash_config_loader(std::string_view uri) {
    auto yaml = ConfigLoader::instance()->load_yaml(uri);
    proxies = yaml["proxies"];
    spdlog::info("Total number of proxies loaded {}", proxies.size());
}

void Subscriber::base64_config_loader(std::string_view uri) {
    auto config = ConfigLoader::instance()->load_raw(uri);
    proxies = decode_config(config);
}

YAML::Node Subscriber::decode_config(std::string_view config) {
    auto decoded_config = Base64::decode(config);
    YAML::Node proxies;

    auto config_list = Utils::split(decoded_config, '\n');
    for (auto &proxy: config_list) {
        try {
            if (!proxy.empty()) {
                Utils::trim(proxy);
                // decode config
                auto uri = Uri::Parse(proxy);
                auto decoder = ProxyDecoderFactory::make(uri.getSchema());
                spdlog::debug("select {} decoder for processing the raw data", uri.getSchema());

                auto proxy_config = decoder->decode_config(uri);
                if (proxy_config.IsDefined()) {
                    proxies.push_back(proxy_config);
                }
            }
        } catch (CSGRuntimeException &e) {
            spdlog::warn("Skip adding proxy {}, due to {}", proxy, e.what());
        }
    }

    return proxies;
}

void Subscriber::grouping(size_t group_min_size) {
    auto netflix_group = node_vector();
    auto ungrouped = node_vector();

    if (enable_grouping) {
        spdlog::info("Grouping proxies by name, minimum size for a group is {}", group_min_size);

        // awaiting delete
        std::vector<std::string> remove_list;

        // grouping
        for (const auto &proxy : proxies) {
            auto proxy_ptr = YAML::Node(proxy);
            // trim proxy name
            proxy_ptr["name"] = Utils::trim_copy(proxy["name"].as<std::string>());
            auto proxy_name = proxy["name"].as<std::string>();
            auto attribute = parse_name(proxy_name);
            const auto& [location, id, netflix, amplification] = attribute;
            spdlog::trace("proxy name: {}, id: {}, netflix: {}, amplification: {}",
                          location, id, netflix, amplification);

            // write attributes to proxy
            append_attributes(attribute, proxy_ptr);

            if (exclude_amplified_node && amplification > 1.0) {
                spdlog::debug("Proxy {} excluded, because the amplification is {}", proxy_name, amplification);
                continue;
            }

            // initialize vector
            if (!group_result.count(location)) {
                group_result.insert({location, node_vector()});
            }

            // insert into map
            group_result.at(location).push_back(proxy);

            // insert into netflix map
            if (netflix) {
                netflix_group.push_back(proxy);
            }
        }

        // post processing
        for (auto const &merge_pair: provider["group_merge"]) {
            auto source_name = merge_pair.first.as<std::string>();
            auto target_name = merge_pair.second.as<std::string>();
            if (group_result.count(source_name) && group_result.count(target_name)) {
                spdlog::debug("Move all proxies in group {} to {}", source_name, target_name);
                auto &source_vec = group_result.at(source_name);
                auto &target_vec = group_result.at(target_name);

                target_vec.insert(target_vec.end(), source_vec.begin(), source_vec.end());
                remove_list.emplace_back(source_name);
            }
        }
        remove_groups(remove_list);

        // move group size > group_min_size to left over
        for (const auto &node: group_result) {
            if (node.second.size() < group_min_size) {
                ungrouped.insert(ungrouped.end(), node.second.begin(), node.second.end());
                remove_list.emplace_back(node.first);
            }
        }
        remove_groups(remove_list);
    } else {
        spdlog::info("Proxy grouping is disabled");
        for (auto proxy : proxies) {
            // trim proxy name
            proxy["name"] = Utils::trim_copy(proxy["name"].as<std::string>());
            ungrouped.emplace_back(proxy);
        }
    }

    if (!netflix_group.empty()) {
        spdlog::debug("Found {} netflix proxies", netflix_group.size());
        group_result.insert({"netflix", netflix_group});
    }

    if (!ungrouped.empty()) {
        spdlog::debug("Found {} ungrouped proxies", ungrouped.size());
        group_result.insert({"Ungrouped", ungrouped});
    }
}

YAML::Node Subscriber::get() {
    auto node = YAML::Node();

    node["groups"] = YAML::Node(YAML::NodeType::Sequence);
    node["proxies"] = YAML::Node(YAML::NodeType::Sequence);
    node["group_name"] = YAML::Node(YAML::NodeType::Sequence);
    if (!group_result.empty()) {
        auto prefix = provider["prefix"].IsDefined() ? provider["prefix"].as<std::string>() : "Generated";
        for (const auto &[name, nodes]: group_result) {
            auto group_name = fmt::format("{}-{}", prefix, name);
            node["group_name"].push_back(group_name);
            spdlog::debug("Processing group {}", group_name);
            auto current_group = YAML::Node();
            current_group["name"] = group_name;
            current_group["proxies"] = YAML::Node(YAML::NodeType::Sequence);
            node["groups"].push_back(current_group);

            // name generator
            auto name_generator = get_name_generator();

            // processing individual proxy
            for (const auto &proxy: nodes) {
                if (proxy.IsDefined() && proxy.IsMap()) {
                    auto proxy_ptr = YAML::Node(proxy);
                    auto proxy_name = proxy["name"];
                    spdlog::trace("Add proxy {} to group {}", proxy_name.as<std::string>(), group_name);

                    // only update name when grouping is enabled
                    if (proxy["attributes"].IsDefined()) {
                        proxy_name = name_generator(proxy);
                    }

                    // do not append duplicated proxy
                    if (name != "netflix") {
                        node["proxies"].push_back(proxy);
                    }
                    // current_group["proxies"].push_back(proxy_name);
                    current_group["proxies"].push_back(proxy_name.as<std::string>());

                    // strip attributes
                    proxy_ptr.remove("attributes");
                }
            }
        }
    }

    return node;
}

std::string Subscriber::name2emoji(const std::string &name) {
    if (emoji_map[name].IsDefined()) {
        return emoji_map[name].as<std::string>();
    }

    spdlog::info("No emoji defined for {}", name);

    return name;
}

Subscriber::NameAttribute Subscriber::parse_name(const std::string &name) {
    std::smatch match;
    NameAttribute attribute{name, -1, false, 1.0f};
    if (std::regex_match(name, match, name_parser)) {
        spdlog::trace("Name {}, total number of matches are {}", name, match.size());
        auto regex_result = get_regex_result(match);

        const auto get_value = [&](const std::string &key_name, const std::string &default_value) {
            auto def_index = provider["definition"][key_name].as<int>();
            if (def_index != -1 && static_cast<long>(regex_result.size()) > def_index) {
                return regex_result[def_index];
            }

            return default_value;
        };

        // do have an emoji mapper
        if (provider["definition"].IsDefined()) {
            auto location = get_value("location_name", "");
            if (!location.empty()) {
                std::get<0>(attribute) = location;
                std::get<1>(attribute) = std::stoi(get_value("position", "-1"));
                std::get<2>(attribute) = !get_value("netflix", "").empty();
                auto amplification = get_value("amplification", "1.0f");
                std::get<3>(attribute) = std::stof(amplification.empty() ? "1.0f" : amplification);
            }
        }
    }

    return attribute;
}

void Subscriber::append_attributes(const Subscriber::NameAttribute &attribute, YAML::Node &node) {
    auto attributes = YAML::Node(YAML::NodeType::Map);
    attributes["location"] = std::get<0>(attribute);
    attributes["id"] = std::get<1>(attribute);
    attributes["netflix"] = std::get<2>(attribute);
    attributes["amplification"] = std::get<3>(attribute);
    node["attributes"] = attributes;
}

std::vector<std::string> Subscriber::get_regex_result(const std::smatch &result) const {
    std::vector<std::string> regex_result;
    std::transform(result.begin(), result.end(), std::back_inserter(regex_result), [](const auto &m) {
        return m.str();
    });

    if (regex_collapse) {
        auto it = std::remove_if(regex_result.begin(), regex_result.end(), [](const auto &m) {
            return m.empty();
        });
        regex_result.erase(it, regex_result.end());
    }

    return regex_result;
}

std::function<std::string(const YAML::Node &)> Subscriber::get_name_generator() {
    return [&](const YAML::Node &proxy) {
        static std::map<std::string, size_t> location_counter;

        auto attributes = proxy["attributes"];
        auto id = attributes["id"].as<int>();
        auto name = attributes["location"].as<std::string>();
        if (id == -1) {
            location_counter.try_emplace(name, 0);
            id = ++location_counter[name];
        }

        return use_emoji ? fmt::format("{}{:>02d}", name2emoji(name), id) : proxy["name"].as<std::string>();
    };
}

void Subscriber::remove_groups(std::vector<std::string> &remove_list) {
    for (const auto &name: remove_list) {
        group_result.erase(name);
        spdlog::debug("Remove group {}", name);
    }
    remove_list.clear();
}

void Subscriber::set_grouping(bool flag) {
    this->enable_grouping = flag;
}

void Subscriber::set_provider(const YAML::Node &_provider) {
    this->provider = _provider;
}

void Subscriber::set_use_emoji(bool _use_emoji) {
    this->use_emoji = _use_emoji;
}

void Subscriber::set_emoji_map(const YAML::Node &_emoji_map) {
    this->emoji_map = _emoji_map;
}

void Subscriber::set_exclude_amplified_node(bool flag) {
    this->exclude_amplified_node = flag;
}

void Subscriber::set_regex_collapse(bool flag) {
    this->regex_collapse = flag;
}

void Subscriber::set_name_parser(const std::string &pattern) {
    this->name_parser = std::regex(pattern, std::regex_constants::icase | std::regex_constants::ECMAScript);
}
