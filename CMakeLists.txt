cmake_minimum_required(VERSION 4.0)
project(untitled)

set(CMAKE_CXX_STANDARD 20)

# macOS Homebrew hint
if(APPLE)
    list(APPEND CMAKE_PREFIX_PATH "/opt/homebrew" "/usr/local")
endif()

find_package(SDL2 REQUIRED)

add_executable(untitled main.cpp
        include/math.hpp
        include/camera.hpp
        include/mesh.hpp
        include/renderer.hpp)

target_link_libraries(untitled PRIVATE SDL2::SDL2)

# On Windows, copy the SDL2.dll next to the executable
if(WIN32)
    add_custom_command(TARGET untitled POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_RUNTIME_DLLS:untitled>
            $<TARGET_FILE_DIR:untitled>
            COMMAND_EXPAND_LISTS
    )
endif()
