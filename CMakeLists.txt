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
LOAD_OPTIONAL(spdlog spdlog)
LOAD_OPTIONAL(fmt fmt)

# submodules
add_compile_definitions(CPPHTTPLIB_OPENSSL_SUPPORT)
add_subdirectory(deps/cpp-httplib)
add_subdirectory(deps/CLI11)

file(GLOB_RECURSE Exceptions "src/exception/*.h")
set(utils src/utils.cpp src/utils.h)
set(RuleExtractor src/rule_extractor.cpp src/rule_extractor.h)
set(ProxyGenerator src/proxy_generator.cpp src/proxy_generator.h)
set(HttpClient src/httpclient.cpp src/httpclient.h)
set(URI src/uri.cpp src/uri.h)
set(base64 src/base64.cpp src/base64.h)
set(YAMLHelper src/yaml_helper.cpp src/yaml_helper.h)
set(ClashSubGenerator src/clash_sub_generator.cpp src/clash_sub_generator.h src/exception/invalid_yaml_excaption.h src/base64.cpp src/base64.h)

add_executable(ClashSubGenerator
        src/main.cpp
        src/config.h
        src/version.h
        ${URI}
        ${HttpClient}
        ${ClashSubGenerator}
        ${RuleExtractor}
        ${ProxyGenerator}
        ${YAMLHelper}
        ${utils}
        ${base64}
        ${Exceptions}
        )
target_link_libraries(ClashSubGenerator yaml-cpp CLI11 fmt spdlog httplib OpenSSL::SSL OpenSSL::Crypto)