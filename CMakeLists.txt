cmake_minimum_required(VERSION 3.20)
project(gargantuan CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# FIXME: SDL2 is doing sm bullshit thats not getting installed thru vcpkg
# Im lazy sooo homebrew time
#
# TODO: luthier for gargantuan
list(APPEND CMAKE_PREFIX_PATH "/opt/homebrew")

file(GLOB_RECURSE GARGANTUAN_SOURCES CONFIGURE_DEPENDS
	"src/*.cpp"
	"src/*.hpp"
	"src/*.h"
)

find_package(SDL2 REQUIRED)
find_package(Vulkan REQUIRED)

add_executable(gargantuan ${GARGANTUAN_SOURCES})

target_link_libraries(gargantuan PRIVATE SDL2::SDL2 Vulkan::Vulkan)
