cmake_minimum_required (VERSION 3.8)

project(VCW)

file(GLOB SOURCES *.c)
add_executable(vk_wrapper ${SOURCES})

find_package(glfw3 REQUIRED)
target_link_libraries(vk_wrapper glfw)

find_package (Vulkan REQUIRED)
target_link_libraries(vk_wrapper Vulkan::Vulkan)

find_package(cglm REQUIRED)
target_link_libraries(vk_wrapper cglm::cglm)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address")
#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address")

#set(RENDERDOC_SDK_PATH "C:/Program Files/RenderDoc")
#include_directories(${RENDERDOC_SDK_PATH})
#target_link_libraries(vk_wrapper "${RENDERDOC_SDK_PATH}/renderdoc.dll")

add_custom_target(vert.spv
	COMMAND glslangValidator --quiet -V ${CMAKE_SOURCE_DIR}/shader.vert -o ${CMAKE_BINARY_DIR}/vert.spv)

add_custom_target(frag.spv
	COMMAND glslangValidator --quiet -V ${CMAKE_SOURCE_DIR}/shader.frag -o ${CMAKE_BINARY_DIR}/frag.spv)
