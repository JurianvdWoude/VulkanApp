
# 3D Graphics Engine made with Vulkan

This creates a window with a moving, rendered 3D scene. 


## Table of Contents

- [Installation](#installation)
- [Usage](#usage)

## Installation

Running this program requires you to build and install Vulkan, GLFW3 and GLM.
Setting up the development environment is based on [The Vulkan Tutorial](https://vulkan-tutorial.com/Development_environment) website.

1. The Vulkan SDK is needed to run this program, which can be downloaded from [the LunarG website](https://vulkan.lunarg.com/). Once installed, you can go to the *bin/* directory and run *vkcube.exe* to test that the SDK works.
2. GLFW can be installed by clicking the download link for [their website](https://www.glfw.org/). Make sure to at least use version 3.3.
3. GLM can be installed by following the [GLM github page](https://github.com/g-truc/glm) and following the instructions at "Build and Install".
4. Finally, run make to build the bin that runs the program.

## Usage

This program is mostly non-interactible, and serves as an example of a running application capable of rendering 3D objects with textures, mipmaps, anti-aliasing, running sound, inputs, and movement.
The 3D scene and textures can be found in the models and textures directories.
Most of the logic can be found in engine.cpp, with the imports being in c_types.h.
All of the inputs, and the window behavior is handled by GLFW.
The audio is handled by FMOD in audio.cpp, and can be altered by pressing a button.

