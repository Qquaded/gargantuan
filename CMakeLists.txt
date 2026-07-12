cmake_minimum_required(VERSION 3.20)
project(gargantuan CXX)

file(GLOB_RECURSE GARGANTUAN_SOURCES CONFIGURE_DEPENDS
	"src/*.cpp"
	"src/*.hpp"
	"src/*.h"
)

find_package(SDL2 REQUIRED)
find_package(Vulkan REQUIRED)

add_executable(gargantuan ${GARGANTUAN_SOURCES})

target_link_libraries(gargantuan PRIVATE SDL2 Vulkan::Vulkan vkbootstrap)
