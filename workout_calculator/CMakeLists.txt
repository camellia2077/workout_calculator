# 设置CMake最低版本要求
cmake_minimum_required(VERSION 3.15)

# 定义项目名称、版本和语言
project(WorkoutParser VERSION 1.0 LANGUAGES CXX)

# 设置C++标准为 C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- 查找整个项目都需要的库 ---
find_package(nlohmann_json REQUIRED)
find_package(SQLite3 REQUIRED)

# --- 新增：定义配置文件的路径 ---
set(CONFIG_FILE_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/src/config/mapping.json)
set(CONFIG_FILE_DESTINATION ${CMAKE_BINARY_DIR})


# ===================================================================
# 目标 1: 交互式菜单应用 (Interactive App)
# ===================================================================

# --- 仅为交互式应用收集其所需的源文件 ---
message(STATUS "Gathering sources for 'workout_tracker_app'...")
file(GLOB COMMON_SOURCES "src/common/*.cpp")
file(GLOB REPROCESSOR_SOURCES 
    "src/reprocessor/*.cpp"
    "src/reprocessor/volume_calculator/*.cpp"
    "src/reprocessor/date_processor/*.cpp"
    "src/reprocessor/log_formatter/*.cpp"
    "src/reprocessor/log_parser/*.cpp"
    "src/reprocessor/name_mapper/*.cpp"
    "src/reprocessor/validator/*.cpp"
)
file(GLOB DB_INSERTER_SOURCES 
    "src/db_inserter/*.cpp"
    "src/db_inserter/_internal/*.cpp"
)
# MODIFIED: 更新 controller 路径
file(GLOB CONTROLLER_SOURCES "src/controller/*.cpp")

add_executable(
    workout_tracker_app          # 可执行文件名
    src/main_app.cpp             # MODIFIED: 更新入口文件路径
    ${COMMON_SOURCES}
    ${REPROCESSOR_SOURCES}
    ${DB_INSERTER_SOURCES}
    ${CONTROLLER_SOURCES}
)

# 为交互式应用设置头文件路径
target_include_directories(
    workout_tracker_app
    PRIVATE
    # MODIFIED: 将 src 目录设为头文件根目录
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# 为交互式应用链接库
target_link_libraries(
    workout_tracker_app
    PRIVATE 
    nlohmann_json::nlohmann_json 
    SQLite::SQLite3 
)

# --- 新增：添加构建后命令，将mapping.json复制到可执行文件目录 ---
add_custom_command(
    TARGET workout_tracker_app
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
            "${CONFIG_FILE_SOURCE}"
            "$<TARGET_FILE_DIR:workout_tracker_app>/mapping.json"
    COMMENT "Copying mapping.json for workout_tracker_app"
)


# ===================================================================
# 目标 2: 命令行工具 (Command-Line Tool)
# ===================================================================

# --- 仅为命令行工具收集其所需的源文件 ---
# 注意：源文件变量在上面已经定义过，这里可以直接复用，无需再次GLOB
message(STATUS "Gathering sources for 'workout_tracker_cli'...")

add_executable(
    workout_tracker_cli          # 可执行文件名
    src/main_cli.cpp             # MODIFIED: 更新入口文件路径
    ${COMMON_SOURCES}
    ${REPROCESSOR_SOURCES}
    ${DB_INSERTER_SOURCES}
    ${CONTROLLER_SOURCES}
)

# 为命令行工具设置头文件路径
target_include_directories(
    workout_tracker_cli
    PRIVATE
    # MODIFIED: 将 src 目录设为头文件根目录
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# 为命令行工具链接库
target_link_libraries(
    workout_tracker_cli
    PRIVATE 
    nlohmann_json::nlohmann_json 
    SQLite::SQLite3 
)

# --- 新增：添加构建后命令，将mapping.json复制到可执行文件目录 ---
add_custom_command(
    TARGET workout_tracker_cli
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
            "${CONFIG_FILE_SOURCE}"
            "$<TARGET_FILE_DIR:workout_tracker_cli>/mapping.json"
    COMMENT "Copying mapping.json for workout_tracker_cli"
)

message(STATUS "Executables will be created at: ${CMAKE_BINARY_DIR}")