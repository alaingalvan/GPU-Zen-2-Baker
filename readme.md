![Cover Image](assets/images/cover.jpg)

# GPU Zen 2 | Ray Casting Techniques for Baked Texture Generation

[![GPU Zen 2][gpuzen-img]][gpuzen-url]
[![cmake-img]][cmake-url]
[![License][license-img]][license-url]
[![Travis Tests][travis-img]][travis-url]

When building real-time applications artists must conform to a performance budget according to their project’s system requirements. Baking allows for the caching of data from geometry with much higher polygon counts to image textures that can be used as inputs in shaders.

This example presents a series of techniques for baking textures to be used in real-time applications. Applications for baking include creating assets with varying levels of detail (LOD), caching lighting effects such as global illumination and diffuse lighting for static assets, and creating atlas maps for objects with multiple materials and UV spaces.

## Getting Started

First install [Git](https://git-scm.com/downloads), then open any terminal such as [Hyper](https://hyper.is/) in any folder and type:

```bash
# 🐑 Clone the repo
git clone https://github.com/alaingalvan/gpu-zen-2-baker --recurse-submodules

# 💿 go inside the folder
cd gpu-zen-2-baker

# 👯 If you forget to `recurse-submodules` you can always run:
git submodule update --init

```

From there we'll need to set up our build files. Be sure to have the following installed:

- [CMake](https://cmake.org/)

- An IDE such as [Visual Studio](https://visualstudio.microsoft.com/downloads/), [XCode](https://developer.apple.com/xcode/), or a compiler such as [GCC](https://gcc.gnu.org/).

Then type the following in your terminal from the repo folder:

```bash
# 👷 Make a build folder for your IDE files
mkdir build
cd build

# 🖼️ To build your Visual Studio solution on Windows x64
cmake .. -A x64

# 🍎 To build your XCode project on Mac OS
cmake .. -G Xcode

# 🐧 To build your .make file on Linux
cmake ..

# 🔨 (Optional) Build your project:
cmake --build .
```

This example was written to work on:

- 🖼️ **Windows** (Tested on *Windows 10* | 3.0Ghz AMD Ryzen 1700 / NVIDIA 1080 / 32GB Ram)

- 🍎 **MacOS** (Tested on *MacOS 10.13 High Sierra* | iMac 2017 / 3.4GHz Intel i5 / Radeon Pro 560 / 16GB Ram)

- 🐧 **Linux** (Tested on *Ubuntu 18.04 Bionic Beaver* | 3.0Ghz Intel Celleron / RX 570 / 4GB Ram)

## Usage

![UI highlights screenshot](assets/images/ui-highlights.png)

When the program starts a UI element will appear to the right that shows all the controls for the baker system.

Click `Load Low Poly Mesh` to load the low poly mesh you'll be applying textures to. Make sure this mesh has UVs.

Click `load High Poly Mesh` to load the high poly mesh you'll be getting your data from.

Click `Choose Output Folder` to change the output folder.

You can set the resolution, cage distance, and which maps to bake from the UI.

Click the `Bake` button to bake those textures and have them written to `<yourpath>/<yourmap>.png`.

### Supported Maps

![Baker Outputs](assets/images/baked-outputs.png)

#### Surface

- ⬆️ Normal

- ↘️ Object Normal

#### Lights

- 🌒 Ambient Occlusion

### Navigation

|      Action       |         Input        |
|------------------:|:--------------------:|
| **Rotate Camera** | `Alt + Left Click`   |
| **Pan Camera**    | `Alt + Middle Click` |
| **Zoom Camera**   | `Alt + Right Click`  |
| **Move Light**    | `Shift + Left Click` |

## Architecture

The file structure for this example is a basic *actor-component engine*, with a middleware component to the engine to handle shared state that actors can subscribe to (such as the graphics window).

```bash
├─ external/
├─ assets/
│  ├─ shaders/
│  │  ├─ Bake.vert                  # Shared Vertex Shader across all baker passes
│  │  ├─ BakeNormals.frag           # Baker pass for normals / object normals
│  │  └─ BakeAmbientOcclusion.frag  # Baker pass for ambient occlusion
│  └─ ...
├─ src/
│  ├─ Engine/
│  │  ├─ Core/                      # Core Engine structures, basic Actor-Component design
│  │  ├─ Actors/                    # Actors, where you'll find entities in the scene
│  │  ├─ Middleware/                # The engine's shared state, such as graphic windows and IMGUI
│  │  ├─ Engine.cpp
│  │  └─ Engine.h
│  └─ XMain.cpp                     # Cross Platform main function
├─ ...
└─ CMakeLists.txt
```

### Execution Order

1. Your platform dependent main function is called, which calls the abstraction main function `void xmain(int argc, const char** argv)`.

2. `xmain` creates an `Engine` to `start()` it.

3. The `Engine` creates middleware that holds state that can be managed independently and shared to `Actor`s. It creates a `CoreMiddleware` that manages events, the window, presentation, and keeps the engine running so long as the window hasn't closed.

4. When the `CoreMiddleware` is updating, it passes the `Engine`'s `Scene` over to the `Renderer`, which performs a preorder traversal of scene hierarchy, updating each actor and rendering it. Afterwords it renders the GUI.

The core logic of the Baker can be found in the `Baker` class located in `/src/Engine/Actors/`, with its shaders found in `/assets/shaders`.

### Dependencies

- [CrossWindow](https://github.com/alaingalvan/CrossWindow) - A Cross platform windowing library.

- [CrossWindow-Graphics](https://github.com/alaingalvan/CrossWindow-Graphics) - Creates and manages OpenGL context.

- [OpenGL-Registry](https://github.com/KhronosGroup/OpenGL-Registry) - Header files for all OpenGL APIs.

- [Glad](https://github.com/Dav1dde/glad) - OpenGL library Loader.

- [VectorMath](https://github.com/glampert/vectormath) - Modified version of Sony's open sourced vector and matrix math library.

- [stb](https://github.com/nothings/stb) - A number of public domain libraries, including a PNG writer / reader that we're using in this example.

- [IMGUI](https://github.com/ocornut/imgui) - a bloat-free graphical user interface library for C++.

- [Open Asset Import Library](https://github.com/assimp/assimp) - Loads 40+ 3D file formats into one unified and clean data structure.

## License

This project is licensed as either **MIT** or **Apache-2.0**, whichever you would prefer.

[gpuzen-img]: https://img.shields.io/badge/gpu%20zen-2-2a2a2a.svg?style=flat-square
[gpuzen-url]: http://gpuzen.blogspot.com/
[cmake-img]: https://img.shields.io/badge/cmake-3.6-1f9948.svg?style=flat-square
[cmake-url]: https://cmake.org/
[license-img]: https://img.shields.io/:license-mit-blue.svg?style=flat-square
[license-url]: https://opensource.org/licenses/MIT
[travis-img]: https://img.shields.io/travis/alaingalvan/GPU-Zen-2-Baker.svg?style=flat-square&logo=travis
[travis-url]: https://travis-ci.org/alaingalvan/GPU-Zen-2-Baker