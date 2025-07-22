# 设置CMake最低版本要求 
cmake_minimum_required(VERSION 3.15) 

# 定义项目名称、版本和语言 
project(WorkoutParser VERSION 1.0 LANGUAGES CXX) 

# 设置C++标准为 C++17 
set(CMAKE_CXX_STANDARD 17) 
set(CMAKE_CXX_STANDARD_REQUIRED ON) 

# --- 查找 nlohmann-json 库 (现代、可靠的方式) ---
# find_package会查找由 pacman 安装的 nlohmann_jsonConfig.cmake 文件
find_package(nlohmann_json REQUIRED)

# 定义可执行文件及其源文件
add_executable(
    workout_tracker
    main.cpp
    log_parser.cpp
    name_mapper.cpp
)

# --- 链接库 ---
# 将 nlohmann_json::nlohmann_json 目标链接到我们的可执行文件
# 这会自动处理头文件包含路径，是现代CMake的推荐做法
target_link_libraries(
    workout_tracker
    PRIVATE
    nlohmann_json::nlohmann_json
)

# 打印出可执行文件位置，方便调试
message(STATUS "Successfully configured workout_tracker.")
message(STATUS "Executable will be created at: ${CMAKE_BINARY_DIR}/workout_tracker.exe")