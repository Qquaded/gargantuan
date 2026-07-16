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
	"vendor/box3d/include"
	"vendor/freetype/include"
	"vendor/glm/glm"
	"vendor/luau/Common/include"
	"vendor/luau/VM/include"
	"vendor/luau/Compiler/include"
	"vendor/luau/Ast/include"
	"vendor/luau/CodeGen/include"
	"vendor/sdl/include"
	"vendor/spdlog/include"
	"vendor/vma/include"
)

add_subdirectory(vendor/box3d)
add_subdirectory(vendor/freetype)
add_subdirectory(vendor/glm)
add_subdirectory(vendor/luau)
add_subdirectory(vendor/sdl) 
add_subdirectory(vendor/spdlog) 
add_subdirectory(vendor/vkbootstrap)
add_subdirectory(vendor/vma)

find_package(Vulkan REQUIRED)

target_link_libraries(gargantuan
	PRIVATE
		SDL3::SDL3
        Vulkan::Vulkan
		box3d
		freetype
		glm
		spdlog::spdlog
		vk-bootstrap::vk-bootstrap
		GPUOpen::VulkanMemoryAllocator
)
