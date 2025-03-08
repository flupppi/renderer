cmake_minimum_required(VERSION 3.30)  # CMake version check
set(CMAKE_TOOLCHAIN_FILE "C:/Users/feli-/.vcpkg-clion/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
set(VCPKG_FEATURE_FLAGS "manifests" CACHE STRING "")

project(CXXOpenGL)               # Create project "simple_example"
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)



find_package(OpenGL REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(GLEW CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)
find_package(Lua REQUIRED MODULE)
message(STATUS "LUA_INCLUDE_DIR = ${LUA_INCLUDE_DIR}")
message(STATUS "LUA_LIBRARIES = ${LUA_LIBRARIES}")




# ------------------------------------------------------------------------------
# 1) Build std.ixx => std.ifc, std.obj
# ------------------------------------------------------------------------------
#
# If Visual Studio is installed in a standard location, you can define:
# set(VCToolsInstallDir "C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.42.34433")
# Or else find it dynamically. We'll just assume it's known:
#
set(VCToolsInstallDir "C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.42.34433")
set(STD_IXX_FILE "${VCToolsInstallDir}/modules/std.ixx")

# The custom command compiles 'std.ixx' into 'std.ifc' + 'std.obj' in our build folder:
add_custom_command(
        OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/std.ifc"
        "${CMAKE_CURRENT_BINARY_DIR}/std.obj"
        COMMAND "${CMAKE_CXX_COMPILER}"
        /c
        /nologo /W4 /EHsc
        # Force the same debug/release flags as your normal build. For example:
        /MDd         # if Debug
        /Od          # if Debug
        /std:c++latest
        /utf-8
        # This is the actual module file from MSVC's installation:
        "${STD_IXX_FILE}"
        # Tell MSVC where to place std.obj & std.ifc:
        /Fo "${CMAKE_CURRENT_BINARY_DIR}/std.obj"
        /ifcOutput "${CMAKE_CURRENT_BINARY_DIR}/std.ifc"
        WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
        COMMENT "Building standard library module 'std' from ${STD_IXX_FILE}"
        VERBATIM
)

# Make a target out of that command so we can depend on it
add_custom_target(std_modules_target
        DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/std.ifc"
        "${CMAKE_CURRENT_BINARY_DIR}/std.obj"
)


# Add main.cpp file of the project root directory as a source file
set(SOURCE_FILES
        main.cpp
        2DGame/GameRenderer.cpp
        Raytracer/RaytracerRenderer.cpp
        vendor/stb_image/stb_image.cpp
)

add_executable(CXXOpenGL ${SOURCE_FILES})
target_sources(CXXOpenGL PRIVATE FILE_SET cxx_modules TYPE CXX_MODULES BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}" FILES
        # And all your .ixx files:
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
        "MeshSimplification/BillboardRenderer.ixx"
        "MeshSimplification/BillboardTexture.ixx"
        "MeshSimplification/BoundingBox.ixx"
        "MeshSimplification/IPlaneSelector.ixx"
        "MeshSimplification/PlaneSelector.ixx"
        "MeshSimplification/PlaneSpace.ixx"
        "Raytracer/Raytracer.ixx"
        "Raytracer/RaytracerRenderer.ixx"
        "SkinnedAnimation/Application.ixx"
        "SkinnedAnimation/Joint.ixx"
        "SkinnedAnimation/Mesh.ixx"
        "SkinnedAnimation/Renderer.ixx"
        "SkinnedAnimation/Skeleton.ixx")

if(MSVC)


    # MSVC-specific compile flags for modules
    target_compile_options(CXXOpenGL PRIVATE
            /std:c++latest
            /reference "std=${CMAKE_CURRENT_BINARY_DIR}/std.ifc"
            /utf-8
    )
endif()
# Add executable target with source files listed in SOURCE_FILES variable
target_compile_definitions(CXXOpenGL PRIVATE GLM_ENABLE_EXPERIMENTAL)
target_include_directories(CXXOpenGL PRIVATE ${LUA_INCLUDE_DIR})
target_link_libraries(CXXOpenGL PRIVATE
        glfw
        GLEW::GLEW
        OpenGL::GL
        assimp::assimp
        glm::glm
        ${LUA_LIBRARIES}
        "${CMAKE_CURRENT_BINARY_DIR}/std.obj"

)