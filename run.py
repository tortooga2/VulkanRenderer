import os
import platform


if platform.system() == "Windows":
    os.system("cmake --build ./build/")
    os.system(".\\build\\Debug\\VulkanEngine.exe")

elif platform.system() == "Darwin":
    os.system("cmake --build ./build/")
    os.system("./build/VulkanEngine")


