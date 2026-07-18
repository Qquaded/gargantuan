cmake_minimum_required(VERSION 3.20)
project(gargantuan CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_program(GLSL_COMPILER glslc REQUIRED)

file(GLOB_RECURSE SHADER_SOURCES CONFIGURE_DEPENDS
	"assets/shaders/*.vert"
	"assets/shaders/*.frag"
)

foreach(SHADER IN LISTS SHADER_SOURCES)
	get_filename_component(SHADER_NAME ${SHADER} NAME)
	set(SPIRV_OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/shaders/${SHADER_NAME}.spv")

	file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/shaders")

	add_custom_command(
		OUTPUT ${SPIRV_OUTPUT}
		COMMAND ${GLSL_COMPILER} ${SHADER} -o ${SPIRV_OUTPUT}
		DEPENDS ${SHADER}
		COMMENT "Compiling ${SHADER_NAME}"
	)

	list(APPEND SPIRV_OUTPUTS ${SPIRV_OUTPUT})
endforeach()

set(LUAU_BUILD_CLI OFF CACHE BOOL "" FORCE)
set(LUAU_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(SDL_TEST OFF CACHE BOOL "" FORCE)
set(SDL_SHARED ON CACHE BOOL "" FORCE)

file(GLOB_RECURSE GARGANTUAN_SOURCES CONFIGURE_DEPENDS
	"src/*.cpp"
	"src/*.hpp"
)

add_executable(gargantuan ${GARGANTUAN_SOURCES} ${SPIRV_OUTPUTS})

target_include_directories(gargantuan PRIVATE
	"./"
	"include"
)

target_include_directories(gargantuan SYSTEM PUBLIC
	"vendor/box3d/include"
	"vendor/freetype/include"
	"vendor/glm/glm"
	"vendor/luau/Common/include"
	"vendor/luau/Compiler/include"
	"vendor/luau/Ast/include"
	"vendor/luau/VM/include"
	"vendor/luau/Require/include"
	"vendor/luau/Config/include"
	"vendor/luau/CodeGen/include"
	"vendor/sdl/include"
	"vendor/spdlog/include"
)

add_subdirectory(vendor/box3d)
add_subdirectory(vendor/freetype)
add_subdirectory(vendor/glm)
add_subdirectory(vendor/luau)
add_subdirectory(vendor/sdl)
add_subdirectory(vendor/spdlog)

target_link_libraries(gargantuan
	PRIVATE
		SDL3::SDL3
		box3d
		freetype
		glm
		spdlog::spdlog
		Luau.Compiler
		Luau.CodeGen
		Luau.Ast
		Luau.VM
		Luau.Require
		Luau.Config
)
