cmake_minimum_required(VERSION 3.5.1)
project(ClashSubGenerator)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif ()

if (CMAKE_BUILD_TYPE MATCHES Release)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_RELEASE} -Os -flto")
elseif (CMAKE_BUILD_TYPE MATCHES Debug)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g")
    enable_testing()
endif (CMAKE_BUILD_TYPE MATCHES Release)

# define version
set(CSG_VERSION_MAJOR 0)
set(CSG_VERSION_MINOR 2)
set(CSG_VERSION_PATCH 0)
set(CSG_DEVELOPMENT ON)
set(CSG_RELEASE_INFO RELEASE)

if (CSG_DEVELOPMENT)
    include(GitVersion)
    set(CSG_RELEASE_INFO ${GIT_VERSION})
endif ()

# generate version.h
include_directories(${CMAKE_BINARY_DIR})
configure_file("${CMAKE_SOURCE_DIR}/src/version.h.in" "${CMAKE_BINARY_DIR}/version.h" @ONLY)

# OpenSSL
find_package(OpenSSL REQUIRED)
message(STATUS "Using OpenSSL ${OPENSSL_VERSION}")
include_directories(${OPENSSL_INCLUDE_DIRS})

# optional
include(LoadOptional)
LOAD_OPTIONAL(Yaml-cpp yaml-cpp)
LOAD_OPTIONAL(fmt fmt)

# submodules
set(SPDLOG_FMT_EXTERNAL ON CACHE BOOL "Use external fmt library instead of bundled" FORCE)
add_subdirectory(deps/spdlog)
add_compile_definitions(CPPHTTPLIB_OPENSSL_SUPPORT)
add_subdirectory(deps/cpp-httplib)
add_subdirectory(deps/CLI11)

file(GLOB_RECURSE Exceptions "src/exception/*.h")
set(utils src/utils.cpp src/utils.h)
set(RuleExtractor src/rule_extractor.cpp src/rule_extractor.h)
set(Subscriber src/subscriber.cpp src/subscriber.h)
set(ClashSubscriber src/clash_subscriber.cpp src/clash_subscriber.h)
set(V2raySubscriber src/v2ray_subscriber.cpp src/v2ray_subscriber.h)
set(ShadowsocksSubscriber src/shadowsocks_subscriber.cpp src/shadowsocks_subscriber.h)
set(HttpClient src/httpclient.cpp src/httpclient.h)
set(URI src/uri.cpp src/uri.h)
set(base64 src/base64.cpp src/base64.h)
set(YAMLHelper src/yaml_helper.cpp src/yaml_helper.h)
set(ClashSubGenerator src/clash_generator.cpp src/clash_generator.h)

add_executable(ClashSubGenerator
        src/main.cpp
        src/config.h
        src/version.h
        ${URI}
        ${HttpClient}
        ${RuleExtractor}
        ${Subscriber}
        ${ClashSubscriber}
        ${V2raySubscriber}
        ${ShadowsocksSubscriber}
        ${YAMLHelper}
        ${utils}
        ${base64}
        ${Exceptions}
        ${ClashSubGenerator}
        )
target_link_libraries(ClashSubGenerator yaml-cpp CLI11 fmt spdlog httplib OpenSSL::SSL OpenSSL::Crypto)