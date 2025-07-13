# C++ OpenGL Projects

This project is a cluster of different rendering applications all using C++ 23 with OpenGL for rendering. 

In the following sections each of the subtopics is described, with some documentation and the current issues and next steps.



- The Program compiles into a executable `CXXOpenGL.exe`.
- This takes arguments. 
- But calling it without parameters currently results in a segmentation fault. This definately can't be right and isn't the intended behavior.

> ! Fix Segmentation fault on empty call of program.
- I see that currently the default is the "Application" program. So this is probably causing a segmentation fault when not called with the right parameters.



I have no idea what I am doing here and how to even proceed in a structured manner. There are so many issues and so many things are broken. And i just don't have even enough knowledge to implement basically anything. So how would I even go about doing such basic things. And then also when i am already tired. 

It is ok. I have slept now and i am back. I have new energy that i can put into this project now.
## Semantic Abstraction Pipeline
The semantic abstraction pipeline is part of a project, where the goal is to load a scene using common formats, e.g. GLTF retrieve the metadata defined on the objects and render both semantic segmentation masks, normal maps, depth maps and other beneficial data sources.

The implementation also includes a viewer for the generated data. The data is stored in bitmaps and also the COCO format for the segmentation masks.

Currently the development of this Project is halted. The issue is that there are already similar projects, that are prebuilt that we first want to try, before committing to developing something new completely from scratch. Probably what it out there will be 90% of what we want to achieve and that would mean that for the rest of the 10 percent you just develop some custom solution but inside the prebuilt environment.

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
It is ok. I have slept now and i am back. I have new energy that i can put into this project now.


## Loading a scene with multiple objects
- glb file with multiple objects in it stored in the gltf format. 
- Assimp can load glb files, provides Scene structure with objects in it. 
- Convert scene structure to own model / scene structure
- Render each model.
