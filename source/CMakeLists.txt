cmake_minimum_required(VERSION 3.30)

set(CMAKE_TOOLCHAIN_FILE "C:/Users/feli-/.vcpkg-clion/vcpkg/scripts/buildsystems/vcpkg.cmake"
        CACHE STRING "")
set(VCPKG_FEATURE_FLAGS "manifests" CACHE STRING "")

project(CXXOpenGL LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ----------------------------------------------------------------------------------------
# 2) Now define your main project/executable
# ----------------------------------------------------------------------------------------
find_package(OpenGL REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(GLEW CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)
find_package(Lua REQUIRED MODULE)
find_package(imgui CONFIG REQUIRED)
find_package(cxxopts CONFIG REQUIRED)
find_path(TINYGLTF_INCLUDE_DIRS "tiny_gltf.h")

message(STATUS "LUA_INCLUDE_DIR = ${LUA_INCLUDE_DIR}")
message(STATUS "LUA_LIBRARIES = ${LUA_LIBRARIES}")

# Source files for your normal .cpp code:
set(SOURCE_FILES
        main.cpp
        2DGame/GameRenderer.cpp
        vendor/stb_image/stb_image.cpp
        tiny_gltf_impl.cpp
)


add_library(MyModules STATIC)
# This FILE_SET approach requires CMake >= 3.26
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




target_compile_definitions(MyModules PRIVATE GLM_ENABLE_EXPERIMENTAL)
target_include_directories(MyModules PRIVATE "${LUA_INCLUDE_DIR}" "${TINYGLTF_INCLUDE_DIRS}")
message(STATUS "TINYGLTF_INCLUDE_DIRS = ${TINYGLTF_INCLUDE_DIRS}")
if(MSVC)
    target_compile_options(MyModules PRIVATE
            /utf-8
            /experimental:module
            /std:c++latest
            /Zi
    )
endif()
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

add_executable(CXXOpenGL ${SOURCE_FILES})
target_link_libraries(CXXOpenGL
        PRIVATE
        MyModules
)
target_compile_definitions(CXXOpenGL PRIVATE GLM_ENABLE_EXPERIMENTAL)
target_include_directories(CXXOpenGL PRIVATE "${LUA_INCLUDE_DIR}" "${TINYGLTF_INCLUDE_DIRS}")
if(MSVC)
    target_compile_options(CXXOpenGL PRIVATE
            /utf-8
            /std:c++latest
            /experimental:module
            /Zi
    )
endif()
# --------------------------
# Tests
# --------------------------
enable_testing()
find_package(Catch2 CONFIG REQUIRED)

add_executable(raytracer_tests
        tests/test_raytracer.cpp
)

target_link_libraries(raytracer_tests
        PRIVATE
        Catch2::Catch2WithMain
        glm::glm
        MyModules
)

include(CTest)
include(Catch)
catch_discover_tests(raytracer_tests)
