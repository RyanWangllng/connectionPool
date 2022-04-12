cmake_minimum_required(VERSION 3.0)
project(connectionPool)

# 配置编译选项
# add_compile_options(-std=c++11)
add_definitions(-std=c++11)
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -g)

# 配置最终的可执行文件输出的路径
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

# 头文件的搜索路径
include_directories(${PROJECT_SOURCE_DIR}/include)
# link_directories(/usr/lib/mysql)

# 加载子目录
add_subdirectory(src)