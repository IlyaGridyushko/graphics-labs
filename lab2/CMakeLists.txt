cmake_minimum_required(VERSION 3.19)
project(lab2 CXX)

set(CMAKE_CXX_STANDARD 17)

# Find required packages
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm CONFIG REQUIRED)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ImGui sources
set(IMGUI_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/imgui/imgui.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/imgui/imgui_demo.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/imgui/imgui_draw.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/imgui/imgui_tables.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/imgui/imgui_widgets.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/imgui/backends/imgui_impl_glfw.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/imgui/backends/imgui_impl_opengl3.cpp
)

# STB Image library (header-only)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/third_party_local/stb)

# Include directories
include_directories(${GLM_INCLUDE_DIRS})
include_directories(${OPENGL_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/third_party/glad/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/GL_helper)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/third_party/imgui)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/third_party/imgui/backends)

# Explicitly add glad.c
set(GLAD_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/third_party/glad/src/glad.c)

# Collect C++ source files
file(GLOB HELPER_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/GL_helper/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/GL_helper/*.h"
)

file(GLOB APP_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
)

# Create the executable
add_executable(
    ${PROJECT_NAME}
    ${APP_SOURCES}
    ${HELPER_SOURCES}
    ${GLAD_SOURCE}
    ${IMGUI_SOURCES}
)

# Link libraries
target_link_libraries(${PROJECT_NAME}
    ${OPENGL_LIBRARIES}
    glfw
    ${CMAKE_DL_LIBS}  # Add dlopen/dlclose support for GLAD
)

# Set proper source file properties for GLAD
set_source_files_properties(${GLAD_SOURCE} PROPERTIES LANGUAGE C)
enable_language(C)

# Tell CMake that the executable depends on the headers
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)
