# recons

This project is a partial **C++** port of my Python project [object-reconstruction](https://github.com/filipondios/object-reconstruction).

>[!WARNING]
> This repository is still under construction and does not yet offer a complete application.

## Building with CMake

First of all, after cloning this project's repository, you need to initialize the git submodules, which in
this case are the dependencies [Catch2 3.10.0](https://github.com/catchorg/Catch2/tree/25319fd3047c6bdcf3c0170e76fa526c77f99ca9),
[Clipper2 1.5.4](https://github.com/AngusJohnson/Clipper2/tree/ef88ee97c0e759792e43a2b2d8072def6c9244e8),
[Lohmann-json 3.12.0](https://github.com/nlohmann/json/tree/55f93686c01528224f448c19128836e7df245f72),
[Eigen 3.4.0](https://gitlab.com/libeigen/eigen),
[Opencv 4.12.0](https://github.com/opencv/opencv/tree/49486f61fb25722cbcf586b7f4320921d46fb38e) and 
[Raylib 5.5](https://github.com/raysan5/raylib/tree/c1ab645ca298a2801097931d1079b10ff7eb9df8).

```bash
git clone https://github.com/filipondios/mceliece
git submodule update --init --recursive
```

The `CMakePresets.json` file supports building for `x64` and `x86` architectures, in any mode `debug` or `release` for 
Linux and Windows. The compilation process is very simple: first you must choose a preset and the compile that preset.
These are some examples:

```sh
# Compile the project for x64 linux release mode
cmake --preset x64-release-linux-x11
cmake --build --preset x64-release-linux-x11

# Compile the project for x86 windows debug mode
cmake --preset x86-debug-windows
cmake --build --preset x86-debug-windows
```
> [!IMPORTANT]
> Since Linux offers the [X11](https://www.x.org/) and [Wayland](https://gitlab.freedesktop.org/wayland/wayland) windowing protocols,
> this project also adds supports for both systems. For example, the presets `x64-debug-linux-x11` and `x64-debug-linux-wayland`
> are equivalent.
