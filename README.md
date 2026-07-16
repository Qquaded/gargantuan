# gargantuan

3D game engine inspired by Kinemiun Engine and ONLY Kinemium Engine.

It's scriptable using Luau!

## Development

Disclaimer: I am stupid and I don't have a fucking clue why you need to do allat

```
brew install cmake vcpkg ninja
```

(Make sure you set up vcpkg)

Install Vulkan SDK as well, then configure CMake:

```
cmake -DCMAKE_TOOLCHAIN_FILE=$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake -DCMAKE_OSX_ARCHITECTURES=arm64 --preset=default
```

Now:

```
cmake --build build && clear && ./build/gargantuan
```
