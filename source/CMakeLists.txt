cmake_minimum_required(VERSION 3.30)
project(CXXOpenGL LANGUAGES CXX)

# --- vcpkg ---
set(CMAKE_TOOLCHAIN_FILE "${CMAKE_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
set(VCPKG_FEATURE_FLAGS "manifests" CACHE STRING "")


set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# --- Experimental Modules Support ---
set(CMAKE_EXPERIMENTAL_CXX_MODULE_CMAKE_API ON)
set(CMAKE_EXPERIMENTAL_CXX_MODULE_DYNDEP ON)


# --- Dependencies ---
find_package(OpenGL REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(GLEW CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)
find_package(Lua REQUIRED MODULE)
find_package(imgui CONFIG REQUIRED)
find_package(cxxopts CONFIG REQUIRED)
find_package(Catch2 CONFIG REQUIRED)
find_path(TINYGLTF_INCLUDE_DIRS "tiny_gltf.h")

message(STATUS "LUA_INCLUDE_DIR = ${LUA_INCLUDE_DIR}")
message(STATUS "LUA_LIBRARIES = ${LUA_LIBRARIES}")
message(STATUS "TINYGLTF_INCLUDE_DIRS = ${TINYGLTF_INCLUDE_DIRS}")


# --- std module precompilation ---
if(MSVC)
        message(STATUS "MSVC handles std module automatically.")
        # MSVC needs only flags
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        add_custom_command(
                OUTPUT std.pcm
                COMMAND ${CMAKE_CXX_COMPILER} -std=c++23 -stdlib=libc++
            -Wno-reserved-identifier -Wno-reserved-module-identifier
            --precompile -o std.pcm /usr/share/libc++/v1/std.cppm
        DEPENDS /usr/share/libc++/v1/std.cppm
        VERBATIM
    )
    add_custom_target(precompile_std_module DEPENDS std.pcm)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    add_custom_command(
        OUTPUT gcm.cache/std.gcm
        COMMAND ${CMAKE_CXX_COMPILER} -std=c++23 -fmodules -fsearch-include-path -c bits/std.cc
        VERBATIM
    )
    add_custom_target(precompile_std_module DEPENDS gcm.cache/std.gcm)
else()
    message(FATAL_ERROR "Unsupported compiler: ${CMAKE_CXX_COMPILER_ID}")
endif()



# --- MyModules (C++23 Modules) ---
add_library(MyModules STATIC)
if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_dependencies(MyModules precompile_std_module)
    target_sources(MyModules PRIVATE std.pcm)
    target_compile_options(MyModules PRIVATE
        -stdlib=libc++
        -fmodule-file=std=std.pcm
    )
    target_link_options(MyModules PRIVATE -stdlib=libc++)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    add_dependencies(MyModules precompile_std_module)
    target_compile_options(MyModules PRIVATE
        -fmodules
    )
endif()

target_sources(MyModules PUBLIC
        FILE_SET cxx_modules TYPE CXX_MODULES
        BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}"
        FILES
        "Camera.ixx"
        "Face.ixx"
        "GameInterface.ixx"
        "GameObject.ixx"
        "IComponent.ixx"
        "IGameObject.ixx"
        "IMaterial.ixx"
        "IMesh.ixx"
        "InputSystem.ixx"
        "IRenderer.ixx"
        "KeyboardObserver.ixx"
        "Model.ixx"
        "Plane.ixx"
        "Quad.ixx"
        "RendererComponent.ixx"
        "SemanticSegmentationRenderer.ixx"
        "SemanticVisualization.ixx"
        "ShaderUtil.ixx"
        "TransformComponent.ixx"
        "Annotation.ixx"
        "CombinedInterface.ixx"
        "2DGame/Game.ixx"
        "2DGame/GameRenderer.ixx"
        "MeshSimplification/Billboard.ixx"
        "MeshSimplification/BillboardCloud.ixx"
        "MeshSimplification/BillboardGenerator.ixx"
        "MeshSimplification/IBillboardGenerator.ixx"
        "MeshSimplification/BillboardRenderer.ixx"
        "MeshSimplification/BillboardTexture.ixx"
        "MeshSimplification/BoundingBox.ixx"
        "MeshSimplification/IPlaneSelector.ixx"
        "MeshSimplification/PlaneSelector.ixx"
        "MeshSimplification/PlaneSpace.ixx"
        "Raytracer.ixx"
        "RaytracerRenderer.ixx"
        "SkinnedAnimation/Application.ixx"
        "SkinnedAnimation/Joint.ixx"
        "SkinnedAnimation/Mesh.ixx"
        "SkinnedAnimation/Renderer.ixx"
        "SkinnedAnimation/Skeleton.ixx"
)

set(VCPKG_INCLUDE_DIR "${CMAKE_BINARY_DIR}/vcpkg_installed/x64-linux/include")

target_include_directories(MyModules PUBLIC
    "${LUA_INCLUDE_DIR}"
    "${TINYGLTF_INCLUDE_DIRS}"
    "${VCPKG_INCLUDE_DIR}"
)
target_compile_definitions(MyModules PRIVATE GLM_ENABLE_EXPERIMENTAL)



target_link_libraries(MyModules PRIVATE
        glfw
        GLEW::GLEW
        OpenGL::GL
        assimp::assimp
        glm::glm
        imgui::imgui
        "${LUA_LIBRARIES}"
        cxxopts::cxxopts
)




# --- Main executable ---
set(SOURCE_FILES
        main.cpp
        2DGame/GameRenderer.cpp
        vendor/stb_image/stb_image.cpp
        tiny_gltf_impl.cpp
)

add_executable(CXXOpenGL ${SOURCE_FILES})

if(MSVC)
    target_compile_options(MyModules PRIVATE
            /utf-8 /experimental:module /std:c++latest /Zi
    )
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_dependencies(CXXOpenGL precompile_std_module)
    target_sources(CXXOpenGL PRIVATE std.pcm)
    target_compile_options(CXXOpenGL PRIVATE -stdlib=libc++ -fmodule-file=std=std.pcm)
    target_link_options(CXXOpenGL PRIVATE -stdlib=libc++)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    add_dependencies(CXXOpenGL precompile_std_module)
    target_compile_options(CXXOpenGL PRIVATE -fmodules)
endif()

target_link_libraries(CXXOpenGL PRIVATE MyModules)
target_include_directories(CXXOpenGL PRIVATE
    "${LUA_INCLUDE_DIR}"
    "${TINYGLTF_INCLUDE_DIRS}"
    "${VCPKG_INCLUDE_DIR}"
)
target_compile_definitions(CXXOpenGL PRIVATE GLM_ENABLE_EXPERIMENTAL)

# --- Tests ---
add_executable(raytracer_tests tests/test_raytracer.cpp)
target_link_libraries(raytracer_tests PRIVATE Catch2::Catch2WithMain glm::glm MyModules)

enable_testing()
include(CTest)
include(Catch)
catch_discover_tests(raytracer_tests)