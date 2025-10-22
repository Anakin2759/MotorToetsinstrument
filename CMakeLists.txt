# 设置CMake最低版本要求
cmake_minimum_required(VERSION 3.20)

# 设置项目名称和版本
project(MCT VERSION 1.0 LANGUAGES CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)



set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
#set(CMAKE_INCLUDE_CURRENT_DIR ON)

# 设置输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# 将src/下的所有C++源文件添加为一个变量
file(GLOB SOURCES
    src/*.cpp
    src/model/*.cpp
    src/view/*.cpp
    src/controller/*.cpp
    src/utils/*.cpp
    src/interface/*.cpp
    
)
# 添加utils下面的头文件
include_directories(utils)


# 引入子目录（例如构建库，或者外部依赖）
add_subdirectory(src)








