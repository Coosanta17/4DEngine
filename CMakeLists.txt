cmake_minimum_required(VERSION 3.31.6)
project(4DEngine)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_LIST_DIR}/cmake/")

find_package(SDL3 REQUIRED)
find_package(cereal REQUIRED)

add_library(4DEngine
        include/math.hpp
        include/Camera3D.hpp
        include/mesh.hpp
        include/renderer.hpp
        include/Camera4D.hpp
        include/Camera.hpp
        include/mesh4d.hpp
        include/AppWindow.hpp
        src/Camera3D.cpp
        src/Camera4D.cpp
        src/mesh4d.cpp
        src/renderer.cpp
        src/AppWindow.cpp
)

# temporary for ease of distribution - planning to use .so/.dll once finished.
target_link_libraries(4DEngine PRIVATE
        sdl::sdl
        cereal::cereal
)

add_executable(FirstPersonTest examples/first_person_test/main.cpp)
target_link_libraries(FirstPersonTest PRIVATE
        4DEngine
        sdl::sdl
        cereal::cereal)

add_executable(GenerateMeshes examples/meshes/generate.cpp)
target_link_libraries(GenerateMeshes PRIVATE 4DEngine cereal::cereal)

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
