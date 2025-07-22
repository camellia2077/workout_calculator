# 设置CMake最低版本要求
cmake_minimum_required(VERSION 3.15)

# 定义项目名称、版本和语言
project(WorkoutParser VERSION 1.0 LANGUAGES CXX)

# 设置C++标准为 C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- 自动查找所有源文件 ---
# 使用 file(GLOB_RECURSE) 递归扫描子目录中的所有 .cpp 文件
# CONFIGURE_DEPENDS 确保在添加或删除源文件后，CMake会自动重新配置
file(GLOB_RECURSE SRC_FILES
    CONFIGURE_DEPENDS
    "reprocessor/data_processor/*.cpp"
    "reprocessor/log_parser/*.cpp"
    "reprocessor/name_mapper/*.cpp"
)

# --- 查找 nlohmann-json 库 ---
find_package(nlohmann_json REQUIRED)

# --- 定义可执行文件 ---
# 将 main.cpp 和上面自动查找到的所有源文件一起添加
add_executable(
    workout_tracker
    main.cpp
    ${SRC_FILES}
)

# --- 为目标单独添加头文件搜索路径 ---
# 这是现代CMake的首选方式，作用域更精确
target_include_directories(
    workout_tracker
    PRIVATE
    # ${CMAKE_CURRENT_SOURCE_DIR} 是项目根目录的绝对路径
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# --- 链接库 ---
target_link_libraries(
    workout_tracker
    PRIVATE
    nlohmann_json::nlohmann_json
)

# 打印出可执行文件位置，方便调试
message(STATUS "Successfully configured workout_tracker.")
message(STATUS "Executable will be created at: ${CMAKE_BINARY_DIR}/workout_tracker.exe")