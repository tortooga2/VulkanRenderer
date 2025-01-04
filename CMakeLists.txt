cmake_minimum_required(VERSION 3.27)

# Set the project name
project(VulkanEngine)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)


set(SOURCE_FILS main.cpp simpleObject.cpp)
set(HEADER_FILS simpleObject.h)



find_package(Vulkan REQUIRED)

add_subdirectory(libs/glfw-3.4)





# Add the executable
add_executable(VulkanEngine ${SOURCE_FILS} ${HEADER_FILS})

target_link_libraries(VulkanEngine PRIVATE Vulkan::Vulkan glfw)
