cmake_minimum_required(VERSION 4.0.0)
set(CMAKE_CXX_COMPILER /usr/bin/clang++)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "a9e1cf81-9932-4810-974b-6eccaf14e457")
set(CMAKE_CXX_MODULE_STD 1)

project(CXXOpenGL VERSION 0.1.0 LANGUAGES CXX)

# --- vcpkg ---
set(CMAKE_TOOLCHAIN_FILE "${CMAKE_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
set(VCPKG_FEATURE_FLAGS "manifests" CACHE STRING "")





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
endif()



# --- MyModules (C++23 Modules) ---
add_library(MyModules STATIC)
if(MSVC)
    target_compile_options(MyModules PRIVATE
            /utf-8 /experimental:module /std:c++latest /Zi
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