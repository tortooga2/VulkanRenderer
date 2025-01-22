import os
import platform

cmake_command = "cmake --build ./build"
run_command = ".\\build\\Debug\\VulkanEngine.exe"
glslc_path = "C:\\VulkanSDK\\1.3.268.0\\Bin\\glslc.exe"
    


if platform.system() == "Darwin":
    glslc_path = "/Users/chasenagle/VulkanSDK/1.3.296.0/macOS/bin/glslc"
    cmake_command = "cmake --build ./build"
    run_command = "./build/VulkanEngine"
    

os.system(f"{glslc_path} ./Shaders/src/shader.vert -o ./shaders/compiled/vert.spv")
os.system(f"{glslc_path} ./Shaders/src/shader.frag -o ./shaders/compiled/frag.spv")
os.system(cmake_command)
os.system(run_command)










