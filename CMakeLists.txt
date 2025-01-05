cmake_minimum_required(VERSION 3.27)
include(FetchContent)

# Set the project name
project(VulkanEngine)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)


set(SOURCE_FILS main.cpp simpleObject.cpp)
set(HEADER_FILS simpleObject.h)


find_package(Vulkan REQUIRED)

if (WIN32)

FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw
        GIT_TAG master
        GIT_SHALLOW TRUE
        SYSTEM
)

FetchContent_MakeAvailable(glfw)

elseif (APPLE)
    find_package(glfw3)
endif ()

# Add the executable
add_executable(VulkanEngine ${SOURCE_FILS} ${HEADER_FILS})



target_include_directories(VulkanEngine PRIVATE ${Vulkan_INCLUDE_DIRS})

target_link_libraries(VulkanEngine PRIVATE glfw Vulkan::Vulkan)
