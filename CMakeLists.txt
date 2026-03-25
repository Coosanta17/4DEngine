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

# temporary for ease of distribution
target_link_libraries(4DEngine PRIVATE SDL2::SDL2main SDL2::SDL2)

# Statically link msvc dlls temporary for ease of distribution
if(WIN32)
    if(MSVC)
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    endif()
    set_target_properties(4DEngine PROPERTIES WIN32_EXECUTABLE ON)
#    add_custom_command(TARGET 4DEngine POST_BUILD
#            COMMAND ${CMAKE_COMMAND} -E copy_if_different
#            $<TARGET_RUNTIME_DLLS:4DEngine>
#            $<TARGET_FILE_DIR:4DEngine>
#            COMMAND_EXPAND_LISTS
#    )
endif()
