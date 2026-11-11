cmake_minimum_required(VERSION 3.25)
project(market_data_engine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Sanitizers: none | address | thread. Applied to everything, incl. GoogleTest.
set(MDE_SANITIZE "none" CACHE STRING "Sanitizer: none, address, thread")
set_property(CACHE MDE_SANITIZE PROPERTY STRINGS none address thread)

add_compile_options(-fno-omit-frame-pointer)
if(MDE_SANITIZE STREQUAL "address")
  add_compile_options(-fsanitize=address,undefined -fno-sanitize-recover=all)
  add_link_options(-fsanitize=address,undefined)
elseif(MDE_SANITIZE STREQUAL "thread")
  add_compile_options(-fsanitize=thread)
  add_link_options(-fsanitize=thread)
elseif(NOT MDE_SANITIZE STREQUAL "none")
  message(FATAL_ERROR "MDE_SANITIZE must be none, address or thread")
endif()

# Warnings for our own code only (not for GoogleTest).
option(MDE_WERROR "Treat warnings as errors" OFF)
add_library(mde_warnings INTERFACE)
target_compile_options(mde_warnings INTERFACE
  -Wall -Wextra -Wpedantic -Wshadow -Werror=return-type
  $<$<BOOL:${MDE_WERROR}>:-Werror>)

# GoogleTest
include(FetchContent)
FetchContent_Declare(googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.17.0.tar.gz)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()
include(GoogleTest)
file(GLOB MDE_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
add_library(mde ${MDE_SOURCES})
target_include_directories(mde PUBLIC include)
target_link_libraries(mde PRIVATE mde_warnings)
add_subdirectory(tests)
