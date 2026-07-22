# gargantuan

Gargantuan is a 3D game-engine scriptable using Luau.

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

### Windows

note: same as above but for Windows and windows is SO BITCHY so good luck.

yoink Visual Studio 2022 (Community's free) with the "Desktop development with C++" worklaod
checkmarked thats where the compiler is. Then winget the rest:

```
winget install Kitware.CMake
winget install Ninja-build.Ninja
winget install --id KhronosGroup.VulkanSDK -e
```

vcpkg has no winget package (which is sooo annoying), so clone it yourself (not inside this repo) and bootstrap it:

```
git clone https://github.com/microsoft/vcpkg.git
cd vcpkg
.\bootstrap-vcpkg.bat
```

Set a `VCPKG_ROOT` env var pointing at that folder.

Don't forget the submodules, or nothing will compile:

```
git submodule update --init --recursive
```

open a **"Developer PowerShell for VS 2022"** (search for it in the Start menu) instead of a
normal terminal, that's what makes `cl.exe` (the MSVC compiler) available, then:

```
cmake -B build -G Ninja -DCMAKE_TOOLCHAIN_FILE=$env:VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake -DCMAKE_BUILD_TYPE=Release
```

that only needs to happen once. after that just build and run from a normal powershell in the
repo root:

```
cmake --build build; clear; ./build/gargantuan
```

(don't run it from inside `build/` it looks for shaders and scripts relative to wherever you
launched it from)

sorry if there is some bad spelling </3
