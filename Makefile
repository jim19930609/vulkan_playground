all:
	clang++ api_test.cpp -std=c++17 -l MoltenVK -I/opt/homebrew/Cellar/molten-vk/1.1.11/libexec/include/ -L/opt/homebrew/Cellar/molten-vk/1.1.11/lib -o api_test
	#clang++ api_test.cpp -std=c++17 -l glfw -l MoltenVK -L/opt/homebrew/Cellar/glfw/3.3.8/lib -I/opt/homebrew/Cellar/glfw/3.3.8/include/ -L/opt/homebrew/Cellar/molten-vk/1.1.11/lib -o api_test
