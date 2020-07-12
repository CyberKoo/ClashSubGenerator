cmake_minimum_required(VERSION 3.10)
project(ClashSubGenerator)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
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
set(CSG_VERSION_PATCH 2)
set(CSG_DEVELOPMENT ON)

if (CSG_DEVELOPMENT)
    include(GitVersion)
    set(CSG_RELEASE_INFO ${GIT_VERSION})
else ()
    set(CSG_RELEASE_INFO RELEASE)
endif ()

# require filesystem module
#find_package(Filesystem REQUIRED)
if (APPLE)
    set(LIBFS c++)
else ()
    set(LIBFS stdc++fs)
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
add_definitions(-DCPPHTTPLIB_OPENSSL_SUPPORT)
#add_compile_definitions(CPPHTTPLIB_OPENSSL_SUPPORT)
add_subdirectory(deps/cpp-httplib)
add_subdirectory(deps/CLI11)

# glob files
file(GLOB_RECURSE Exceptions "src/exception/*.h")
file(GLOB_RECURSE ProxyDecoderHeaders "src/proxy_decoder/*.h")
file(GLOB_RECURSE ProxyDecoderSources "src/proxy_decoder/*.cpp")
file(GLOB_RECURSE SubscriberHeaders "src/subscriber/*.h")
file(GLOB_RECURSE SubscriberSources "src/subscriber/*.cpp")

#
set(Subscriber ${SubscriberHeaders} ${SubscriberSources})
set(ProxyDecoder ${ProxyDecoderHeaders} ${ProxyDecoderSources})
set(utils src/utils.cpp src/utils.h)
set(RuleExtractor src/rule_extractor.cpp src/rule_extractor.h)
set(HttpClient src/httpclient.cpp src/httpclient.h)
set(URI src/uri.cpp src/uri.h)
set(Hash src/hash.cpp src/hash.h)
set(FileSystem src/filesystem.cpp src/filesystem.h)
set(version src/version.cpp src/version.h)
set(base64 src/base64.cpp src/base64.h)
set(YAMLHelper src/yaml_helper.cpp src/yaml_helper.h)
set(ClashSubGenerator src/clash_generator.cpp src/clash_generator.h)

add_executable(ClashSubGenerator
        src/main.cpp src/config.h src/enum_mapper.h src/enumerate.h
        ${URI} ${HttpClient} ${RuleExtractor} ${YAMLHelper} ${version} ${Exceptions}
        ${ProxyDecoder} ${Subscriber} ${utils} ${Hash} ${FileSystem} ${base64} ${ClashSubGenerator}
        )
target_link_libraries(ClashSubGenerator PRIVATE yaml-cpp CLI11 fmt spdlog httplib OpenSSL::SSL OpenSSL::Crypto ${LIBFS})