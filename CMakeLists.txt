# Project Info

cmake_minimum_required(VERSION 3.6 FATAL_ERROR)
cmake_policy(VERSION 3.6)
project(BakingTextures
        VERSION 1.0.0.0
        LANGUAGES C CXX
)

# =============================================================

# CMake Settings

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_SUPPRESS_REGENERATION true)
set(DCMAKE_GENERATOR_PLATFORM "x64")
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/..)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/..)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/..)
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/..)
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/..)
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/..)
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/..)
if(NOT CMAKE_DEBUG_POSTFIX)
  set(CMAKE_DEBUG_POSTFIX d)
endif()

# =============================================================

# Core Dependencies

# CrossWindow
add_subdirectory(external/crosswindow)
set_property(TARGET CrossWindow PROPERTY FOLDER "Dependencies")

# CrossWindow-Graphics
set(XGFX_API OPENGL CACHE STRING "Which graphics API to use.")
add_subdirectory(external/crosswindow-graphics)

# Glad
add_subdirectory(external/glad)
target_include_directories(
    Glad
    PUBLIC external/opengl-registry/api
)
set_property(TARGET Glad PROPERTY FOLDER "Dependencies")

# IMGUI
set(
  IMGUI_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui/imgui.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui/imgui_draw.cpp
)
add_library(
  IMGUI
  "${IMGUI_SOURCES}"
)

target_include_directories(
  IMGUI
  PUBLIC "exteral/stb"
  PUBLIC "external/imgui"
)

set_property(TARGET IMGUI PROPERTY FOLDER "Dependencies")

# Assimp
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
set(ASSIMP_LIB_INSTALL_DIR ${CMAKE_BINARY_DIR}/bin CACHE STRING "" FORCE)
if (POLICY CMP0048)
  # CMake warns if loaded from a min-3.0-required parent dir, so silence the warning:
  cmake_policy(SET CMP0048 OLD)
endif()
add_subdirectory(external/assimp)
set(ASSIMP_LIBRARY_DIRS "lib/assimp/build/code")
set(ASSIMP_INCLUDE_DIRS "lib/assimp/include" "lib/assimp/build/include")
set_property(TARGET assimp uninstall UpdateAssimpLibsDebugSymbolsAndDLLs IrrXML PROPERTY FOLDER "Dependencies/Assimp")
set_property(TARGET zlib zlibstatic  PROPERTY FOLDER "Dependencies/Zlib")

# =============================================================

# Sources

file(GLOB_RECURSE FILE_SOURCES RELATIVE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h
)

# Solution Filters
foreach(source IN LISTS FILE_SOURCES)
    get_filename_component(source_path "${source}" PATH)
    string(REPLACE "/" "\\" source_path_msvc "${source_path}")
    string(REPLACE "src" "" source_path_final "${source_path_msvc}")
    source_group("${source_path_final}" FILES "${source}")
endforeach()

# =============================================================

# Finalize App

xwin_add_executable(
    ${PROJECT_NAME}
    "${FILE_SOURCES}"
)

# =============================================================

# Finish Dependencies

target_link_libraries(
    ${PROJECT_NAME}
    Glad
    CrossWindowGraphics
    CrossWindow
    IMGUI
    assimp
)

target_include_directories(
  ${PROJECT_NAME}
  PUBLIC "external/vectormath"
  PUBLIC "exteral/stb"
  PUBLIC "external/assimp/include"
  PUBLIC "${CMAKE_BINARY_DIR}/external/assimp/include"
)

target_compile_definitions(
  ${PROJECT_NAME} 
  PUBLIC XGFX_${XGFX_PROTOCOL}=1
)

# =============================================================

# Finish Settings

# Change output dir to bin
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/..
)

if (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
  # Change working directory to top directory to access the `assets/` folder
  set_property(TARGET ${PROJECT_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/..)

  # Change startup project to this project
  set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})
endif()