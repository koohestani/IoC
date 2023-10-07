cmake_minimum_required(VERSION 3.16)

project(ioc
    VERSION "1.0.0"
    DESCRIPTION "Impl inversion of control."
    LANGUAGES C CXX)

file(GLOB SRCS 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
)

add_executable(${PROJECT_NAME} ${SRCS})

target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# static link gcc
option(STATIC_LINK_GCC "Link gcc lib static" ON)
if (STATIC_LINK_GCC)
    message(STATUS "Static linking with libgcc and libstdc++")
    set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")
    string(APPEND CMAKE_SHARED_LINKER_FLAGS "-static-libgcc -static-libstdc++")
endif()

# force colored output
option (FORCE_COLORED_OUTPUT "Always produce ANSI-colored output (GNU/Clang only)." ON)
if (${FORCE_COLORED_OUTPUT})
    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        add_compile_options (-fdiagnostics-color=always)
    elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
        add_compile_options (-fcolor-diagnostics)
    endif ()
endif ()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_FIND_PACKAGE_PREFER_CONFIG ON)

# set target compile definitions to linux
target_compile_definitions(${PROJECT_NAME} PUBLIC LINUX)

# set build directory
if(CMAKE_BUILD_TYPE MATCHES Release OR CMAKE_BUILD_TYPE MATCHES RelWithDebInfo OR CMAKE_BUILD_TYPE MATCHES MinSizeRel)
    set(BUILD_CONFIG "s64")
else() # Any debug build
    set(BUILD_CONFIG "s64d")
endif()
set(BIN_DIR_PATH "${CMAKE_CURRENT_SOURCE_DIR}/out/bin/${BUILD_CONFIG}")

# set target properties
set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD_REQUIRED ON
    CXX_STANDARD 20
    ARCHIVE_OUTPUT_DIRECTORY "${BIN_DIR_PATH}"
    LIBRARY_OUTPUT_DIRECTORY "${BIN_DIR_PATH}"
    RUNTIME_OUTPUT_DIRECTORY "${BIN_DIR_PATH}"
)

# add target options
target_compile_options(${PROJECT_NAME}
    PRIVATE
        $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:
        -Wall
        -Wextra
        -Wconversion
        # -Wshadow
        -pedantic
        -Wnon-virtual-dtor
        -Wno-long-long
        -pedantic-errors>
)