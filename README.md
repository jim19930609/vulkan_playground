# Compile on Linux

## 1. Install Dependencies
[Vulkan SDK]
https://vulkan.lunarg.com/doc/view/latest/linux/getting_started_ubuntu.html

[GLM]
```
sudo apt install libglm-dev
```

[GLFW]
```
sudo apt-get install libglfw3
sudo apt-get install libglfw3-dev
```

## 2. Build
```
mkdir build
cd build && cmake .. && make -j
```
