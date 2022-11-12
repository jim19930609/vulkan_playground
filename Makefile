all:
	clang++ main.cpp -std=c++17 -l glfw -l MoltenVK -DNDEBUG -I./stb -L/opt/homebrew/Cellar/glfw/3.3.8/lib -I/opt/homebrew/Cellar/glfw/3.3.8/include/ -I/usr/local/include -L/usr/local/lib -I/opt/homebrew/Cellar/glm/0.9.9.8/include -L/opt/homebrew/Cellar/glm/0.9.9.8/lib -o main
clean:
	rm -rf main
