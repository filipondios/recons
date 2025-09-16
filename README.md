# recons

This project is a **C++** partial port of my Python project [object-reconstruction](https://github.com/filipondios/object-reconstruction).

## Building with CMake

First of all, after cloning this project's repository, you need to initialize the git submodules, which in
this case are the dependencies, [Lohmann-json 3.12.0](https://github.com/nlohmann/json/tree/55f93686c01528224f448c19128836e7df245f72),
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

## Program Execution

After compiling the project, the executable will be located at `out/build/<preset>/bin/recons`. 

```bash
recons [-h] -p <path> [-r <resolution>] [-i]
```

| Parameter | Required           | Description                                                                                          |
|:---------:|:------------------:|:-----------------------------------------------------------------------------------------------------|
| `-p`      | :white_check_mark: | Path to the model to be reconstructed.                                                               |
| `-r`      | :x:                | Voxel space resolution. Higher resolution leads to more accurate reconstruction (default = 16).      |
| `-i`      | :x:                | Displays additional information about the reconstructed model after the process ends.                |
| `-h`      | :x:                | Shows a help message with information aboout the program.                                            |

Once you know how to run the program, you can try it with some test objects, which are located in the [models](models) directory.
As you can see, there are two subdirectories ([valid](models/valid) and [tests](models/tests)), which contain different models. To verify the 
correct functioning of the program, try the models stored in valid.

>[!NOTE]
> Based on the valid example models, you can create new models on your own, using the templates stored in the [models](models) directory.



