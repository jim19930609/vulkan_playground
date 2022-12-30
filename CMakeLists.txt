cmake_minimum_required(VERSION 3.17)

project(vulkan_tutorial)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_VISIBILITY_PRESET default)

################################
# Prepare third party packages #
################################
find_package(Vulkan REQUIRED)
message("Using Vulkan Include Dir: ${Vulkan_INCLUDE_DIRS}")
message("Using Vulkan Lib: ${Vulkan_LIBRARIES}")

find_package(glm REQUIRED)
message("Using GLM Include Dir: ${GLM_INCLUDE_DIRS}")
# target_link_libraries(<your executable> glm::glm)

find_package(glfw3 REQUIRED)
message("Using GLFW Include Dir: ${GLFW3_INCLUDE_DIRS}")
# target_link_libraries(myapp glfw)

add_executable(vulkan_app main.cpp)
target_link_libraries(vulkan_app 
                      ${Vulkan_LIBRARIES}
                      glm::glm
                      glfw)
target_include_directories(vulkan_app PUBLIC
                           ${Vulkan_LIBRARIES}
                           ${CMAKE_SOURCE_DIR})
