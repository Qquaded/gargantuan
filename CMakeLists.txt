cmake_minimum_required(VERSION 3.20)
project(gargantuan CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(LUAU_BUILD_CLI OFF CACHE BOOL "" FORCE)
set(LUAU_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(SDL_TEST OFF CACHE BOOL "" FORCE)
set(SDL_SHARED ON CACHE BOOL "" FORCE)

file(GLOB_RECURSE GARGANTUAN_SOURCES CONFIGURE_DEPENDS
	"src/*.cpp"
	"src/*.hpp"
)

add_executable(gargantuan ${GARGANTUAN_SOURCES})

target_include_directories(gargantuan PRIVATE
	"./"
	"include"
	"vendor"
	"vendor/box3d/include"
	"vendor/freetype/include"
	"vendor/glm/glm"
	"vendor/luau/Common/include"
	"vendor/luau/VM/include"
	"vendor/luau/Compiler/include"
	"vendor/luau/Ast/include"
	"vendor/luau/CodeGen/include"
	"vendor/sdl/include"
	"vendor/vkbootstrap/src"
)

add_subdirectory(vendor/box3d EXCLUDE_FROM_ALL)
add_subdirectory(vendor/freetype EXCLUDE_FROM_ALL)
add_subdirectory(vendor/glm EXCLUDE_FROM_ALL)
add_subdirectory(vendor/luau EXCLUDE_FROM_ALL)
add_subdirectory(vendor/sdl EXCLUDE_FROM_ALL) 
add_subdirectory(vendor/vkbootstrap EXCLUDE_FROM_ALL)

find_package(Vulkan REQUIRED)

target_link_libraries(gargantuan
	PRIVATE
		SDL3::SDL3
        Vulkan::Vulkan
		box3d
		freetype
		glm
		VulkanBootstrap
)
