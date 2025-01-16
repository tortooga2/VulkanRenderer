cmake_minimum_required(VERSION 3.27)
include(FetchContent)

# Set the project name
project(VulkanEngine)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)





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

FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm
    GIT_TAG master
    GIT_SHALLOW TRUE
    SYSTEM
)
FetchContent_MakeAvailable(glm)



elseif (APPLE)
    find_package(glfw3)
    find_package(glm)
endif ()


file(GLOB_RECURSE SOURCES "src/*.cpp")

# Add the executable
add_executable(VulkanEngine main.cpp ${SOURCES})

target_include_directories(VulkanEngine PRIVATE ${CMAKE_SOURCE_DIR}/includes)

target_include_directories(VulkanEngine PRIVATE ${Vulkan_INCLUDE_DIRS} ${GLFW_INCLUDE_DIRS} ${GLM_INCLUDE_DIRS})


target_link_libraries(VulkanEngine PRIVATE glfw glm Vulkan::Vulkan)
