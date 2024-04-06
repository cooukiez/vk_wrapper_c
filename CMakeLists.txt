cmake_minimum_required(VERSION 3.15)

project(vulkan C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

file(TO_CMAKE_PATH $ENV{GLFW_64} GLFW_DIR)
file(TO_CMAKE_PATH $ENV{VULKAN_SDK} VULKAN_DIR)
file(TO_CMAKE_PATH $ENV{GLM_C} GLM_DIR)

add_executable(vk_wrapper
	main.c
	vk_core.c
	render.c
	descriptor.c
	buffer.c)

if(UNIX AND NOT APPLE)
	#[[
		Build for linux, basically you don't need to do anything,
		but if you ONLY BUILD glfw or vulkanSDK
		without install it into '/usr' directory, you should

			1. add extra 'target_include_directories' option
			2. modify the 'target_link_libraries' option

		based on your build directories
	]]
	target_link_libraries(vk_wrapper
		/usr/lib/libvulkan.so
		/usr/lib/libglfw.so)
elseif(WIN32)
	#[[
		Build for windows, PLEASE USE MinGW,
		DO NOT USE visual c++ beacuse it doesn't fully support c99
		especially 'Variable Length Array' feature

		Before build, you should

			1. modify the 'target_include_directories' option
				if it's different from the path provided below.

			2. modify the 'target_link_libraries' option
				if it's different from the library file provided below.

		based on your install directories
	]]
	target_include_directories(vk_wrapper PRIVATE
		${VULKAN_DIR}/Include
		${GLFW_DIR}/include
		${GLM_DIR}/include/
		${CMAKE_SOURCE_DIR})

	target_link_libraries(vk_wrapper
		${VULKAN_DIR}/Lib/vulkan-1.lib
		${GLFW_DIR}/lib-mingw-w64/libglfw3dll.a)

	target_link_options(vk_wrapper PRIVATE
		-mwindows)
endif()

add_custom_target(vert.spv
	COMMAND glslangValidator --quiet -V ${CMAKE_SOURCE_DIR}/shader.vert -o ${CMAKE_BINARY_DIR}/vert.spv)

add_custom_target(frag.spv
	COMMAND glslangValidator --quiet -V ${CMAKE_SOURCE_DIR}/shader.frag -o ${CMAKE_BINARY_DIR}/frag.spv)

if(UNIX AND NOT APPLE)
	add_dependencies(vk_wrapper
		vert.spv
		frag.spv)
elseif(WIN32)
	#[[
		Windows need glfw3.dll to run this program,
		modify the path to source dll to your own path
	]]
	add_custom_target(glfw3.dll
		COMMAND ${CMAKE_COMMAND} -E copy ${GLFW_DIR}/lib-mingw-w64/glfw3.dll ${CMAKE_BINARY_DIR}/)

	add_dependencies(vk_wrapper
		vert.spv
		frag.spv
		glfw3.dll)
endif()
