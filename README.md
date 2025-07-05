# recons

>[!WARNING]
> This repository is still under construction and does not yet offer a complete application.

This project is a **C++** port of my Python project [object-reconstruction](https://github.com/filipondios/object-reconstruction). This project uses [CMake](https://github.com/Kitware/CMake) as the build system and
[vcpkg](https://github.com/microsoft/vcpkg) for dependency management.

## Dependencies
All dependencies are managed with vcpkg. They are listed in [vcpkg.json](vcpkg.json):
```json
{
  "dependencies": [
    { "name": "clipper2", "use": "polygon clipping" },
    { "name": "raylib", "use": "3D model rendering" },
    { "name": "nlohmann-json", "use": "config parsing" },
    { "name": "opencv4", "use": "image processing" },
    { "name": "eigen3", "use": "linear algebra (ec. systems)" },
    { "name": "catch2", "use": "tests" }
  ]
}
```

## Building with CMake and vcpkg
> [!IMPORTANT]
> The project is designed to work seamlessly with vcpkg. Make sure you have vcpkg installed and the `$VCPKG_ROOT`
> environment variable is set to your vcpkg path (e.g., `C:\\path\\to\\vcpkg` on Windows or `/path/to/vcpkg` on Linux),
> otherwise you won't be able to compile the project.

Once you are sure that you have vcpkg and the `$VCPKG_ROOT` variable set, you can clone this repository and
download and install all the project dependencies:
```sh
git clone https://github.com/filipondios/recons.git
cd recons
vcpkg install
```

### Windows (Visual Studio)
You can open the project folder directly in **Visual Studio** on Windows without any modifications.
Visual Studio automatically detects the CMakePresets and vcpkg integration. It supports building and running
in all configurations (x64, x86, Debug and Release).

### Linux
Similar to Windows, there are existing presets for compiling the project (you can see them in
[CMakePresets.json](CMakePresets.json)). Once you choose a
preset, you can compile the project. For example:
```sh
# Compile the project for x64 Release mode
cmake --preset x64-release-linux
cmake --build --preset x64-release-linux

# Compile the project for x86 Debug mode
cmake --preset x86-debug-linux
cmake --build --preset x86-debug-linux
```

Once you run the command `cmake --preset <preset>`, the `out/build/<preset>/compile_commands.json`
file will be created. This file is used by the [clangd](https://github.com/clangd/clangd) LSP
to provide C++ IDE features to many editors. However, this file needs to be in the
root of the project. The best option is to create a symlink to the file:
```sh
cd /path/to/recons
PRESET="x64-release-linux"
cmake --preset $PRESET
ln -s out/build/$PRESET/compile_commands.json compile_commands.json
```

> [!NOTE]
> After running build commands (either in Windows or Linux) with a preset `<preset>`, you should find the application executable file at `out/build/<preset>/` and the tests executable file at `out/build/<preset>/tests/`. The application binary must be named `recons` and the tests binary `recons_tests`.


