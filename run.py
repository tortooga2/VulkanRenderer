import os
import platform

cmake_command = "cmake --build ./build"
run_command = ".\\build\\Debug\\VulkanEngine.exe"
glslc_path = "C:\\VulkanSDK\\1.3.268.0\\Bin\\glslc.exe"


if platform.system() == "Darwin":
    cmake_command = "cmake --build ./build"
    run_command = "./build/VulkanEngine"
    


os.system(cmake_command)
os.system(run_command)










