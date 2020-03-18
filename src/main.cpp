//
// Created by Kotarou on 2020/3/15.
//
#include <iostream>

#include <CLI/CLI.hpp>
#include <spdlog/spdlog.h>

#include "config.h"
#include "clash_sub_generator.h"
#include "exception/csg_exeption.h"

int main(int argc, char *argv[]) {
    Config config;
    CLI::App cliApp{"Clash Subscription Generator"};
    cliApp.add_option("-c,--config", config.config_file, "Load configuration file")
            ->default_val("sys_config.yaml")->required(false);
    cliApp.add_option("-w,--working_directory", config.working_directory, "Set default working directory")
            ->required(false)->default_val("./");
    cliApp.add_option("-s,--subscribe_url", config.subscribe_url, "Subscription url")->required(true);
    cliApp.add_option("-n,--provider_name", config.provider_name, "Provider name")->required(false);
    cliApp.add_option("-r,--rules_url", config.rules_uri, "External Rules url")
            ->default_val("https://raw.githubusercontent.com/ConnersHua/Profiles/master/Clash/Pro.yaml");
    cliApp.add_flag("-g,--grouping", config.enable_grouping, "Grouping proxies by name")
            ->default_val(false)->needs("--provider_name")->required(false);
    cliApp.add_flag("-e,--emoji", config.use_emoji, "Use emoji instead of location name")
            ->default_val(false)->required(false);
    cliApp.add_flag("-E,--exclude_amplified_proxy", config.exclude_amplified_proxy, "Exclude amplified proxy")
            ->default_val(true)->required(false);
    cliApp.add_option("-m,--min_group_size", config.group_min_size, "Minimum size of a group")
            ->required(false)->default_val(2)->check(CLI::Range(1, 10));
    cliApp.add_option("-o,--output", config.output, "Output file name")
            ->required(false)->default_val("config.yaml");
    cliApp.add_option("-t,--template", config.template_file, "Template file name")
            ->required(false)->default_val("template.yaml");
    cliApp.add_option("-R,--repository_url", config.repository_url, "Repository base url")
            ->required(false)->default_val("https://raw.githubusercontent.com/CyberKoo/Config/master");
    cliApp.add_flag("-l,--local_only", config.local_only, "Do not load file from remove repository")
            ->required(false)->default_val(false);
    // syntax
    std::map<std::string, Syntax> syntax_mapper({{"MODERN", Syntax::MODERN},
                                                 {"LEGACY", Syntax::LEGACY}});
    cliApp.add_option("-S,--syntax", config.syntax, "Syntax of generated file")->required(false)
            ->default_val(Syntax::MODERN)->transform(CLI::CheckedTransformer(syntax_mapper, CLI::ignore_case));
    // generator
    std::map<std::string, Generator> generator_mapper({{"CONFIG",   Generator::CONFIG},
                                                       {"PROVIDER", Generator::PROVIDER}});
    cliApp.add_option("-G,--generator", config.generator, "Generated format")->required(false)
            ->default_val(Generator::CONFIG)->transform(CLI::CheckedTransformer(generator_mapper, CLI::ignore_case));
    // debug mode
    cliApp.add_flag("-d,--debug", config.debug, "Enable debug mode")
            ->default_val(false)->required(false);

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
        exit(-1);
    }

    return 0;
}