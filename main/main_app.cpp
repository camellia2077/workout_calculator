#include "controller/ActionHandler.h" // 引入 ActionHandler 和 AppConfig
#include <iostream>
#include <string>
#include <optional>
#include <filesystem>
#include <limits>

/**
 * @brief 清理标准输入缓冲区。
 * 在读取失败或需要忽略多余输入时调用。
 */
void clearCinBuffer() {
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

int main(int argc, char* argv[]) {
    // 1. 打印欢迎界面
    std::cout << "=================================================" << std::endl;
    std::cout << "    Workout Log Processor - Interactive Mode" << std::endl;
    std::cout << "=================================================" << std::endl;
    std::cout << std::endl;

    AppConfig config;
    std::string year_input_str;

    // 2. 获取日志文件路径 (带循环验证)
    while (true) {
        std::cout << "[步骤 1/2] 请输入您的日志文件路径: ";
        std::getline(std::cin, config.log_filepath);

        // 验证文件是否存在且不是一个目录
        if (std::filesystem::exists(config.log_filepath) && !std::filesystem::is_directory(config.log_filepath)) {
            break; // 文件有效，跳出循环
        } else {
            std::cerr << "\n[错误] 文件未找到或路径无效。请检查后重试。\n" << std::endl;
        }
    }
    std::cout << "-> 日志文件已设定: " << config.log_filepath << std::endl << std::endl;

    // 3. 获取可选的年份信息 (带循环验证)
    while (true) {
        std::cout << "[步骤 2/2] 请输入4位数的年份 (例如 2024), 或直接按 Enter 键使用当年: ";
        std::getline(std::cin, year_input_str);

        if (year_input_str.empty()) {
            config.specified_year = std::nullopt; // 用户未输入，使用可选类型的空值
            std::cout << "-> 好的，将使用当前系统年份。" << std::endl << std::endl;
            break;
        } else {
            try {
                int year = std::stoi(year_input_str);
                // 进行一个简单的合理性检查
                if (year > 1900 && year < 3000) {
                    config.specified_year = year;
                    std::cout << "-> 年份已设定为: " << year << std::endl << std::endl;
                    break;
                } else {
                    std::cerr << "\n[错误] 请输入一个合理的年份 (如 1900-3000之间)。\n" << std::endl;
                }
            } catch (const std::exception& e) {
                std::cerr << "\n[错误] 输入无效，请输入纯数字年份。\n" << std::endl;
            }
        }
    }

    // 4. 自动填充剩余的配置路径
    std::cout << "正在自动配置其余路径..." << std::endl;
    std::filesystem::path exe_path = argv[0];
    config.db_path = (exe_path.parent_path() / "workouts.sqlite3").string();
    config.mapping_path = "mapping.json";
    std::cout << "-> 数据库路径: " << config.db_path << std::endl;
    std::cout << "-> 映射文件路径: " << config.mapping_path << std::endl;
    std::cout << std::endl;

    // 5. 创建并运行 ActionHandler
    std::cout << "配置完成，开始处理...\n" << std::endl;
    ActionHandler handler;
    bool success = handler.run(config);

    // 6. 显示最终结果并等待用户退出
    if (success) {
        std::cout << "\n处理成功完成！" << std::endl;
    } else {
        std::cerr << "\n处理失败。请检查上方的错误信息。" << std::endl;
    }

    std::cout << "\n按 Enter 键退出...";
    // 使用 std::cin.get() 等待用户按键，防止控制台窗口立即关闭
    // 如果之前有输入失败，可能需要先清空缓冲区
    if (std::cin.peek() == '\n') {
        clearCinBuffer();
    }
    std::cin.get(); 

    return success ? 0 : 1;
}