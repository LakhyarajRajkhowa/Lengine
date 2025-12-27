cmake_minimum_required(VERSION 3.10)
project(Lengine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ────────────────────────────────────────────────
# Output dirs
# ────────────────────────────────────────────────
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# ────────────────────────────────────────────────
# Third Party include dirs
# ────────────────────────────────────────────────
set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/src/external/imgui)
set(SDL2_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/src/external/SDL2)
set(GL_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/src/external/GL)
set(GLM_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/src/external/glm)
set(ASSIMP_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/src/external/assimp)
set(SDL2_LIB_DIR ${CMAKE_SOURCE_DIR}/lib)

# ────────────────────────────────────────────────
# ImGui source files
# ────────────────────────────────────────────────
set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/backends/imgui_impl_sdl2.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)


# ────────────────────────────────────────────────
# Source Collection (lowercase folder structure)
# ────────────────────────────────────────────────

file(GLOB_RECURSE CORE_SRC
    "src/core/*.cpp"
    "src/core/*.h"
)
file(GLOB_RECURSE ASSETS_SRC
    "src/assets/*.cpp"
    "src/assets/*.h"
)
file(GLOB_RECURSE EDITOR_SRC
    "src/editor/*.cpp"
    "src/editor/*.h"
)

file(GLOB_RECURSE PLATFORM_SRC
    "src/platform/*.cpp"
    "src/platform/*.h"
)

file(GLOB_RECURSE GRAPHICS_SRC
    "src/graphics/*.cpp"
    "src/graphics/*.h"
)
file(GLOB_RECURSE LOGGING_SRC
    "src/logging/*.cpp"
    "src/logging/*.h"
)

file(GLOB_RECURSE RESOURCES_SRC
    "src/resources/*.cpp"
    "src/resources/*.h"
)

file(GLOB_RECURSE SCENE_SRC
    "src/scene/*.cpp"
    "src/scene/*.h"
)
file(GLOB_RECURSE UTILS_SRC
    "src/utils/*.h"
    "src/utils/*.cpp"
)
file(GLOB_RECURSE EXTERNAL_SRC
    "src/external/picopng.h"
    "src/external/tinyfiledialogs.h"
    "src/external/picopng.cpp"
    "src/external/tinyfiledialogs.c"
    "src/external/json.hpp"
    "src/external/stb_image.h"
)


set(MAIN_SRC src/main.cpp)

# ────────────────────────────────────────────────
# Visual Studio Filters
# ────────────────────────────────────────────────
source_group(TREE ${CMAKE_SOURCE_DIR}/src PREFIX "" FILES
    ${ASSETS_SRC}
    ${CORE_SRC}
    ${EDITOR_SRC}
    ${PLATFORM_SRC}
    ${GRAPHICS_SRC}
    ${LOGGING_SRC}
    ${RESOURCES_SRC}
    ${SCENE_SRC}
    ${UTILS_SRC}
    ${EXTERNAL_SRC}
)



# ────────────────────────────────────────────────
# Build Executable
# ────────────────────────────────────────────────
add_executable(${PROJECT_NAME}
    ${ASSETS_SRC}
    ${MAIN_SRC}
    ${CORE_SRC}
    ${EDITOR_SRC}
    ${PLATFORM_SRC}
    ${GRAPHICS_SRC}
    ${LOGGING_SRC}
    ${RESOURCES_SRC}
    ${SCENE_SRC}
    ${UTILS_SRC}
    ${EXTERNAL_SRC}
    ${IMGUI_SOURCES}
)

# ────────────────────────────────────────────────
# Include dirs
# ────────────────────────────────────────────────
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/external
    ${GL_INCLUDE_DIR}
    ${GLM_INCLUDE_DIR}
    ${ASSIMP_INCLUDE_DIR}
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
    ${SDL2_INCLUDE_DIR}
)

# ────────────────────────────────────────────────
# Link libraries
# ────────────────────────────────────────────────
target_link_directories(${PROJECT_NAME} PRIVATE ${SDL2_LIB_DIR})

target_link_libraries(${PROJECT_NAME}
    assimp-vc143-mt
    SDL2main
    SDL2
    glew32
    opengl32     
)
