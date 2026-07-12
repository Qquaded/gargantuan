# gargantuan

A comprehensive 3D game engine, scriptable using Luau.

## Development

Disclaimer: I am stupid and I don't have a fucking clue why you need to do allat

```
brew install cmake vcpkg ninja
```

(Make sure you set up vcpkg)

Install Vulkan SDK as well, then configure CMake:

```
cmake --preset=default
```

Now:

```
cmake --build build && clear && ./build/gargantuan
```
