cmake_minimum_required(VERSION 3.31.6)
project(4DEngine)

set(CMAKE_CXX_STANDARD 20)

# macOS Homebrew hint
if(APPLE)
    list(APPEND CMAKE_PREFIX_PATH "/opt/homebrew" "/usr/local")
endif()

find_package(SDL2 CONFIG REQUIRED)

add_executable(4DEngine main.cpp
        include/math.hpp
        include/Camera3D.hpp
        include/mesh.hpp
        include/renderer.hpp
        include/Camera4D.hpp
        include/Camera.hpp)

target_link_libraries(4DEngine PRIVATE SDL2::SDL2main SDL2::SDL2)

# On Windows, copy the SDL2.dll next to the executable
if(WIN32)
    set_target_properties(4DEngine PROPERTIES WIN32_EXECUTABLE ON)
    add_custom_command(TARGET 4DEngine POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_RUNTIME_DLLS:4DEngine>
            $<TARGET_FILE_DIR:4DEngine>
            COMMAND_EXPAND_LISTS
    )
endif()
