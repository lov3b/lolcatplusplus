cmake_minimum_required(VERSION 3.22)
project(lolcat VERSION 2.2.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # Makes development easier

# default to Release
if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "No build type specified: defaulting to Release")
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif ()

if (BUILD_STATIC)
    set(Boost_USE_STATIC_LIBS ON)
    # Make sure that we enable the Static Runtime /MT
    if (MSVC)
        cmake_policy(SET CMP0091 NEW)
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    endif()
endif()

include(CheckIncludeFileCXX)
check_include_file_cxx("format" HAS_FORMAT)
if(NOT HAS_FORMAT)
    find_package(fmt REQUIRED)
endif()
find_package(Boost COMPONENTS program_options REQUIRED)

configure_file(
        "${PROJECT_SOURCE_DIR}/src/version.hpp.in"
        "${PROJECT_BINARY_DIR}/version.hpp"
        @ONLY
)
include_directories("${PROJECT_BINARY_DIR}")

file(GLOB_RECURSE SOURCES "${PROJECT_SOURCE_DIR}/src/*.cpp")

add_executable(${CMAKE_PROJECT_NAME} ${SOURCES})

target_link_libraries(${CMAKE_PROJECT_NAME} LINK_PUBLIC ${Boost_LIBRARIES})
if(NOT HAS_FORMAT)
    target_compile_definitions(${PROJECT_NAME} PRIVATE USE_LIBFMT)
    target_link_libraries(${PROJECT_NAME} PRIVATE fmt::fmt)
endif()

# Compilator options & Warnings
if (MSVC)
    add_compile_definitions(
        _CRT_SECURE_NO_WARNINGS
        _CRT_NONSTDC_NO_DEPRECATE
    )
    target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE /W4 /WX)
else ()
    target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic -Werror)
    if (BUILD_STATIC AND NOT APPLE)
        target_link_options(${CMAKE_PROJECT_NAME} PRIVATE -static-libgcc -static-libstdc++)
    endif()
endif ()

install(TARGETS ${CMAKE_PROJECT_NAME}
        RUNTIME DESTINATION bin
)
