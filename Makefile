all:
	clang++ main.cpp -std=c++17 -l glfw -l MoltenVK -DNDEBUG -L/opt/homebrew/Cellar/glfw/3.3.8/lib -I/opt/homebrew/Cellar/glfw/3.3.8/include/ -I/usr/local/include -L/usr/local/lib -o main
