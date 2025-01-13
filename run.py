import os
import subprocess


print("Compiling Shaders...")
os.system("C:\\VulkanSDK\\1.3.268.0\\Bin\\glslc.exe .\\Shaders\\src\\shader.vert -o .\\Shaders\\compiled\\vert.spv")
os.system("C:\\VulkanSDK\\1.3.268.0\\Bin\\glslc.exe .\\Shaders\\src\\shader.frag -o .\\Shaders\\compiled\\\\frag.spv")

# os.system("mkdir ./build/Shaders/")
# os.system("copy .\\Shaders\\compiled\\vert.spv .\\build\\Shaders\\vert.spv")
# os.system("copy .\\Shaders\\compiled\\frag.spv .\\build\\Shaders\\frag.spv")

print("Building Project...")
os.system("cmake --build ./build/")
print("-------------------RUNNING PROGRAM -------------------")
os.system(".\\build\\Debug\\VulkanEngine.exe")

