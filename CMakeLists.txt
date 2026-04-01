cmake_minimum_required(VERSION 3.31.6)
project(4DEngine)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_LIST_DIR}/cmake/")

# macOS Homebrew hint
if (APPLE)
    list(APPEND CMAKE_PREFIX_PATH "/opt/homebrew" "/usr/local")
endif ()

find_package(SDL2 CONFIG REQUIRED)
find_package(flatbuffers CONFIG REQUIRED)

set(FB_SCHEMA
        "${CMAKE_CURRENT_SOURCE_DIR}/resources/mesh4d.fbs"
)
set(GENERATED_DIR "${CMAKE_CURRENT_SOURCE_DIR}/generated")
file(MAKE_DIRECTORY "${GENERATED_DIR}")

find_program(FLATC_EXECUTABLE NAMES flatc
    HINTS
        ${CMAKE_SOURCE_DIR}/vcpkg_installed/x64-linux/tools/flatbuffers
)
add_custom_command(
        OUTPUT "${GENERATED_DIR}/mesh4d_generated.h"
        COMMAND "${FLATC_EXECUTABLE}" --cpp --gen-mutable -o "${GENERATED_DIR}" "${FB_SCHEMA}"
        DEPENDS "${FB_SCHEMA}"
        COMMENT "Generating C++ headers from FlatBuffers schema"
)

add_custom_target(GenerateFlatBuffers ALL
        DEPENDS "${GENERATED_DIR}/mesh4d_generated.h"
)

add_executable(4DEngine main.cpp
        include/math.hpp
        include/Camera3D.hpp
        include/mesh.hpp
        include/renderer.hpp
        include/Camera4D.hpp
        include/Camera.hpp
        include/mesh4d.hpp
        "${GENERATED_DIR}/mesh4d_generated.h")

add_dependencies(4DEngine GenerateFlatBuffers)
target_include_directories(4DEngine PRIVATE "${GENERATED_DIR}")

# temporary for ease of distribution
target_link_libraries(4DEngine PRIVATE SDL2::SDL2main SDL2::SDL2 flatbuffers::flatbuffers)

# Statically link msvc dlls temporary for ease of distribution
if (WIN32)
    if (MSVC)
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    endif ()
    set_target_properties(4DEngine PROPERTIES WIN32_EXECUTABLE ON)
    #    add_custom_command(TARGET 4DEngine POST_BUILD
    #            COMMAND ${CMAKE_COMMAND} -E copy_if_different
    #            $<TARGET_RUNTIME_DLLS:4DEngine>
    #            $<TARGET_FILE_DIR:4DEngine>
    #            COMMAND_EXPAND_LISTS
    #    )
endif ()
