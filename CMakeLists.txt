cmake_minimum_required(VERSION 3.14 FATAL_ERROR)
project(ClashSubGenerator)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2 -s -flto")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g -ggdb -gdwarf-3")

OPTION(STATIC OFF "Compile standalone binary file")

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif ()

# define version
set(CSG_VERSION_MAJOR 0)
set(CSG_VERSION_MINOR 3)
set(CSG_VERSION_PATCH 1)
set(CSG_DEVELOPMENT OFF)

if (CSG_DEVELOPMENT)
    include(GitVersion)
    set(CSG_RELEASE_INFO ${GIT_VERSION})
else ()
    set(CSG_RELEASE_INFO RELEASE)
endif ()

if (STATIC)
    message(STATUS "Build statically linked binary")
    set(OPENSSL_USE_STATIC_LIBS TRUE)
    SET(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static")
    set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")
endif ()

# generate version.h
include_directories(${CMAKE_BINARY_DIR})
configure_file("${CMAKE_SOURCE_DIR}/src/version.h.in" "${CMAKE_BINARY_DIR}/version.h" @ONLY)

## submodules ##
# filesystem
find_package(Filesystem REQUIRED)

# zlib
find_package(ZLIB REQUIRED)

#openssl
find_package(OpenSSL REQUIRED)
message(STATUS "Using OpenSSL ${OPENSSL_VERSION}")

# optional
include(LoadOptional)
LOAD_OPTIONAL(Yaml-cpp yaml-cpp)
LOAD_OPTIONAL(fmt fmt)

# spdlog
set(SPDLOG_FMT_EXTERNAL ON CACHE BOOL "Use external fmt library instead of bundled" FORCE)
add_subdirectory(deps/spdlog)
# define spdlog logging level
add_compile_definitions(SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_DEBUG)

# CLI11
set(CLI11_BUILD_DOCS OFF)
set(CLI11_BUILD_TESTS OFF)
set(CLI11_BUILD_EXAMPLES OFF)
set(CLI11_BUILD_EXAMPLES_JSON OFF)
set(CLI11_SINGLE_FILE_TESTS OFF)
add_subdirectory(deps/CLI11)

# cpp-httplib
add_compile_definitions(CPPHTTPLIB_OPENSSL_SUPPORT)
set(HTTPLIB_USE_BROTLI_IF_AVAILABLE OFF)
add_subdirectory(deps/cpp-httplib)
## submodules ##

# glob files
file(GLOB_RECURSE Exceptions "src/exception/*.h")
file(GLOB_RECURSE ProxyDecoderHeaders "src/proxy_decoder/*.h")
file(GLOB_RECURSE ProxyDecoderSources "src/proxy_decoder/*.cpp")

# source code
set(ProxyDecoder ${ProxyDecoderHeaders} ${ProxyDecoderSources})
set(Subscriber src/subscriber.cpp src/subscriber.cpp)
set(utils src/utils.cpp src/utils.h)
set(RuleExtractor src/rule_extractor.cpp src/rule_extractor.h)
set(HttpClient src/httpclient.cpp src/httpclient.h)
set(URI src/uri.cpp src/uri.h)
set(Hash src/hash.cpp src/hash.h)
set(version src/version.cpp src/version.h)
set(base64 src/base64.cpp src/base64.h)
set(YAMLHelper src/yaml_helper.cpp src/yaml_helper.h)
set(ConfigLoader src/config_loader.cpp src/config_loader.h)
set(ClashSubGenerator src/clash_generator.cpp src/clash_generator.h)

add_executable(ClashSubGenerator
        src/main.cpp src/config.h src/enum_mapper.h src/enumerate.h src/filesystem.h
        ${URI} ${HttpClient} ${RuleExtractor} ${YAMLHelper} ${version} ${Exceptions} ${ConfigLoader} ${ProxyDecoder}
        ${Subscriber} ${utils} ${Hash} ${base64} ${ClashSubGenerator}
        )
target_link_libraries(ClashSubGenerator PRIVATE yaml-cpp CLI11 fmt spdlog httplib OpenSSL::SSL OpenSSL::Crypto std::filesystem ZLIB::ZLIB)