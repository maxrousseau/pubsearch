cmake_minimum_required(VERSION 3.7)

project(pubsearch VERSION 0.0
                  DESCRIPTION "search pubmed"
                  LANGUAGES CXX)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake) # Include Conan-generated file
conan_basic_setup(TARGETS) # Introduce Conan-generated targets

add_executable(pbsr main.cpp)
target_link_libraries(pbsr CONAN_PKG::cpr CONAN_PKG::fmt)
