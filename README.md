# recons 

This project is a **C++** port of my Python project [object-reconstruction](https://github.com/filipondios/object-reconstruction). This project uses [CMake](https://github.com/Kitware/CMake) as  the build system and 
[vcpkg](https://github.com/microsoft/vcpkg) for dependency management.

## Dependencies

All dependencies are managed with vcpkg. They are listed in [vcpkg.json](vcpkg.json):

- **opencv4** (image processing)
- **raylib** (3D Models rendering)
- **clipper2** (poligon clipping)
- **tomlplusplus** (configuration parsing)
- **eigen3** (linear algebra)

## Building with CMake and vcpkg

> [!IMPORTANT]
> The project is designed to work seamlessly with vcpkg. Make sure you have vcpkg and the `$VCPKG_ROOT`
> enviroment variable is set to your path to vcpkg (ex. `C:\\path\\to\\vcpkg` in Windows or `/path/to/vcpkg` in Linux),
> otherwise you won't be able to compile the project.

Once you are shure that you have vcpkg and the `$VCPKG_ROOT` variable set, you can clone this repository and
download and install all this project dependencies at `vcpkg_installed/`:

```sh
git clone https://github.com/filipondios/recons.git
cd recons
vcpkg install
```

### Windows (Visual Studio)

You can open the project folder directly in **Visual Studio** on Windows without any modifications.
Visual Studio automatically detects the CMakePresets and vcpkg integration. Supports building and running 
in all configurations (x64, x86, Debug and Release).

### Linux

In the same way as windows, there are some existing presets for compiling the project (you can see them at 
[CMakePresets.json](CMakePresets.json)). Once you choose one 
preset, you can compile the project. For example:

```sh
# Compile the project for x64 Release mode
cmake --preset x64-release-linux
cmake --build --preset x64-release-linux

# Compile the project for x86 Debug mode
cmake --preset x86-debug-linux
cmake --build --preset x86-release-linux
```

