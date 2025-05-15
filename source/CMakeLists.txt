cmake_minimum_required(VERSION 3.30)

set(CMAKE_TOOLCHAIN_FILE "C:/Users/feli-/.vcpkg-clion/vcpkg/scripts/buildsystems/vcpkg.cmake"
        CACHE STRING "")
set(VCPKG_FEATURE_FLAGS "manifests" CACHE STRING "")

project(CXXOpenGL LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ----------------------------------------------------------------------------------------
# 0) Locate or define the path to MSVC's std.ixx from your local Visual Studio install
#    E.g. check your actual MSVC version, or detect automatically
# ----------------------------------------------------------------------------------------
set(VCToolsInstallDir "C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.43.34808")
set(STD_IXX_FILE "${VCToolsInstallDir}/modules/std.ixx")

# ----------------------------------------------------------------------------------------
# 1) Custom command: build std.ixx => std.ifc, std.obj
# ----------------------------------------------------------------------------------------
#
# Here, we assume a Debug-like build: /MDd /Od /Zi, but adjust for your actual config.
# We also pass /experimental:module because many MSVC versions require it to compile modules.
#
add_custom_command(
        OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/std.ifc"
        "${CMAKE_CURRENT_BINARY_DIR}/std.obj"
        COMMAND "${CMAKE_CXX_COMPILER}"
        /experimental:module      # needed for many MSVC versions
        /std:c++latest
        /EHsc
        /nologo
        /W4
        /MDd        # If building Debug with /MDd
        /Od         # If Debug -> no optimization
        /Zi         # Generate debug info
        /utf-8
        /c
        "${STD_IXX_FILE}"
        /Fo${CMAKE_CURRENT_BINARY_DIR}/std.obj
        /ifcOutput${CMAKE_CURRENT_BINARY_DIR}/std.ifc
        WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
        COMMENT "Building standard library module 'std' => std.ifc + std.obj"
        VERBATIM
)

# Create a phony target that depends on these files
add_custom_target(std_modules_target
        DEPENDS
        "${CMAKE_CURRENT_BINARY_DIR}/std.ifc"
        "${CMAKE_CURRENT_BINARY_DIR}/std.obj"
)

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

if(MSVC)
    target_compile_options(MyModules PRIVATE
            # Many MSVC versions still need /experimental:module to properly treat .ixx as modules
            /experimental:module
            /std:c++latest
            /utf-8
            /Zi
            # Reference the just-built "std.ifc":
            /reference "std=${CMAKE_CURRENT_BINARY_DIR}/std.ifc"
    )
endif()


# Make sure we build std_modules_target (std.ifc/std.obj) BEFORE compiling your modules
add_dependencies(MyModules std_modules_target)
target_compile_definitions(MyModules PRIVATE GLM_ENABLE_EXPERIMENTAL)
target_include_directories(MyModules PRIVATE "${LUA_INCLUDE_DIR}" "${TINYGLTF_INCLUDE_DIRS}")
message(STATUS "TINYGLTF_INCLUDE_DIRS = ${TINYGLTF_INCLUDE_DIRS}")
target_link_libraries(MyModules PRIVATE
        glfw
        GLEW::GLEW
        OpenGL::GL
        assimp::assimp
        glm::glm
        imgui::imgui
        "${LUA_LIBRARIES}"
        # Link the newly built std.obj so that 'import std;' resolves actual library code
        "${CMAKE_CURRENT_BINARY_DIR}/std.obj"
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

# Link with Catch2 and anything the test needs from your project
target_link_libraries(raytracer_tests
        PRIVATE
        Catch2::Catch2WithMain
        glm::glm
        MyModules
        # Optionally, shared code like Raytracer or modules
)

# Optional: add test discovery with CTest
include(CTest)
include(Catch)
catch_discover_tests(raytracer_tests)
