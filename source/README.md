# C++ OpenGL Projects

This project is a cluster of different rendering applications all using C++ 23 with OpenGL for rendering. 

In the following sections each of the subtopics is described, with some documentation and the current issues and next steps.



- The Program compiles into a executable `CXXOpenGL.exe`.
- This takes arguments. 
- But calling it without parameters currently results in a segmentation fault. This definately can't be right and isn't the intended behavior.

> ! Fix Segmentation fault on empty call of program.
- I see that currently the default is the "Application" program. So this is probably causing a segmentation fault when not called with the right parameters.





## Semantic Abstraction Pipeline
- Create window.
- load image of render - stb image 
- load annotations from json - nlohmann json
- add different visualization modes
- visualize data from annotations


## General C++ Hurdles
- ~~Install dependencies with vcpkg~~ (This works pretty good by now, with the CMake Project setup integrating new packages using VCpkg is straightforward.

## Raytracer
- I need something to take reference from, so i can work on the code from that onwards.
- There are these task descriptions for the tutorials that we could summarize on nbdev. They explain the tasks and how to go about with solving them.
- Let's see.



## Loading a scene with multiple objects
- glb file with multiple objects in it stored in the gltf format. 
- Assimp can load glb files, provides Scene structure with objects in it. 
- Convert scene structure to own model / scene structure
- Render each model.
