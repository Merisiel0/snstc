> ⚠️ **This project is under development.**  
> The code in the main branch will either not compile or not work as expected.
> If you're looking for the last stable commit, you can find it [here](https://github.com/Merisiel0/snstc/tree/4d6cbd23b2a010b155f95391021fa8d42805d0c4).
> Be aware that it will fail to load materials, since images are not included in this repository.

# Sphere (name subject to change)

snstc is a game engine developped during my studies. It's currently only a rendering engine with plans for ray tracing and physics simulation.

## Tech used:

C++, Vulkan, SDL3, EnTT, GLM, vcpkg, CMake, stb

## Dependencies:

You must have installed:
* vcpkg
* CMake
* Ninja
* g++

Environment variables:
* VCPKG_ROOT should be set to vcpkg's executable's path.
* VCPKG_ROOT should be added to PATH.

## Build and run
(vscode and visual studio are confirmed to work)

1. Clone this repository
2. Naviguate to the project's base directory
3. Execute the following command block:
```
cmake -S . -B build              && \
cmake --build build --parallel   && \
./build/Sphere"
```

## Funky little accidents
![damn-the-lighting](https://github.com/Merisiel0/snstc/blob/master/README_assets/2025-04-12%2022-47-07.gif)
![is-this-color-theory-?](https://github.com/Merisiel0/snstc/blob/master/README_assets/color_theory_i_guess.gif)
