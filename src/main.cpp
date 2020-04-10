//
// Created by Kotarou on 2020/3/15.
//
#include <iostream>

#include <CLI/CLI.hpp>
#include <spdlog/spdlog.h>

#include "config.h"
#include "enum_mapper.h"
#include "clash_generator.h"
#include "exception/csg_exeption.h"

int main(int argc, char *argv[]) {
    Config config;
    CLI::App cliApp{"Clash Subscription Generator"};
    auto sub_conf = cliApp.add_option_group("Subscription options");
    auto type = sub_conf->add_option("-T,--subscribe_type", config.subscribe_type, "Subscription type")
            ->required(true)->transform(
                    CLI::CheckedTransformer(EnumMapper::get_subscribe_type_mapper(), CLI::ignore_case));
    auto provider_name = sub_conf->add_option("-n,--provider_name", config.provider_name, "Provider name")
            ->required(false);
    sub_conf->add_option("-s,--subscribe_url", config.subscribe_url, "Subscription url")
            ->needs(type)->required(true);

    auto gen_opts = cliApp.add_option_group("Generator options");
    gen_opts->add_flag("-e,--emoji", config.use_emoji, "Use emoji instead of location name")
            ->default_val(false)->required(false);
    gen_opts->add_flag("-g,--grouping", config.enable_grouping, "Grouping proxies by name")
            ->default_val(false)->needs(provider_name)->required(false);
    gen_opts->add_flag("-E,--exclude_amplified_proxy", config.exclude_amplified_proxy, "Exclude amplified proxy")
            ->default_val(true)->required(false);
    gen_opts->add_option("-m,--min_group_size", config.group_min_size, "Minimum size of a group")
            ->required(false)->default_val(2)->check(CLI::Range(1, 10));
    gen_opts->add_option("-r,--rules_url", config.rules_uri, "External Rules url")
            ->default_val("https://raw.githubusercontent.com/ConnersHua/Profiles/master/Clash/Pro.yaml");
    gen_opts->add_option("-S,--syntax", config.syntax, "Syntax of generated file")
            ->required(false)->default_val(Syntax::MODERN)
            ->transform(CLI::CheckedTransformer(EnumMapper::get_syntax_mapper(), CLI::ignore_case));
    gen_opts->add_option("-G,--generator", config.generator, "Generated format")->required(false)
            ->default_val(Generator::CONFIG)->transform(
                    CLI::CheckedTransformer(EnumMapper::get_generator_mapper(), CLI::ignore_case));

    cliApp.add_option("-c,--config", config.config_file, "Load configuration file")
            ->default_val("sys_config.yaml")->required(false);
    cliApp.add_option("-w,--working_directory", [&config](const CLI::results_t &res) -> bool {
        if (res[0].back() != '/' || res[0].back() != '\\') {
            config.working_directory = fmt::format("{}/", res[0]);
        } else {
            config.working_directory = res[0];
        }
        return true;
    }, "Set default working directory")->required(false)->default_val("./");
    cliApp.add_option("-o,--output", config.output, "Output file name")
            ->required(false)->default_val("config.yaml");
    cliApp.add_option("-t,--template", config.template_file, "Template file name")
            ->required(false)->default_val("template.yaml");
    cliApp.add_option("-R,--repository_url", config.repository_url, "Repository base url")
            ->required(false)->default_val("https://raw.githubusercontent.com/CyberKoo/Config/master");
    cliApp.add_flag("-l,--local_only", config.local_only, "Do not load file from remove repository")
            ->required(false)->default_val(false);
    // debug mode
    cliApp.add_flag("-d,--debug", config.debug, "Enable debug mode")
            ->default_val(false)->required(false);
    // display version and exit
    cliApp.add_flag_callback("-V,--version", []() -> void {
        std::cout << ClashSubGenerator::version() << std::endl;
        std::exit(0);
    }, "Print version")->required(false);

    CLI11_PARSE(cliApp, argc, argv);

    // logging
    spdlog::set_pattern("[%Y-%m-%d %H:%M:%S %z] [%^%l%$] %v");

    if (config.debug) {
        spdlog::set_level(spdlog::level::debug);
        spdlog::debug("Verbose mode enabled");
    } else {
        spdlog::set_level(spdlog::level::info);
    }

    ClashSubGenerator generator(config);
    try {
        generator.run();
    } catch (CSGRuntimeException &e) {
        spdlog::critical("{}", e.what());
        std::abort();
    }

    return 0;
}