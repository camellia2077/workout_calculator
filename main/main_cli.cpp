#include "controller/ActionHandler.h"
#include <iostream>
#include <optional>
#include <string>
#include <filesystem>

// 将 printUsage 和 parseArguments 作为 main.cpp 内的自由函数
void printUsage(const char* programName) {
    std::cerr << "Usage: " << programName << " --path <log_file.txt> [--year <YYYY>]" << std::endl;
    std::cerr << std::endl;
    std::cerr << "Description:" << std::endl;
    std::cerr << "  Processes a workout log file, generates a reprocessed text file," << std::endl;
    std::cerr << "  and automatically saves the data to 'workouts.sqlite' in the program's directory." << std::endl;
    std::cerr << std::endl;
    std::cerr << "Options:" << std::endl;
    std::cerr << "  -p, --path <file>      Required: Path to the workout log file." << std::endl;
    std::cerr << "  -y, --year <year>      Optional: Specify a year for processing. Defaults to current year." << std::endl;
    std::cerr << "  -h, --help             Show this help message and exit." << std::endl;
}

std::optional<AppConfig> parseCommandLine(int argc, char* argv[]) {
    if (argc == 1) {
        printUsage(argv[0]);
        return std::nullopt;
    }

    AppConfig config;
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-h" || arg == "--help") {
            printUsage(argv[0]);
            return std::nullopt;
        } else if ((arg == "-p" || arg == "--path") && i + 1 < argc) {
            config.log_filepath = argv[++i];
        } else if ((arg == "-y" || arg == "--year") && i + 1 < argc) {
            try {
                config.specified_year = std::stoi(argv[++i]);
            } catch (const std::exception& e) {
                std::cerr << "Error: Invalid year format provided." << std::endl;
                return std::nullopt;
            }
        } else {
            std::cerr << "Error: Unknown or invalid argument '" << arg << "'" << std::endl;
            printUsage(argv[0]);
            return std::nullopt;
        }
    }

    if (config.log_filepath.empty()) {
        std::cerr << "Error: Log file path is required. Use --help for more information." << std::endl;
        return std::nullopt;
    }

    // 在这里构建动态路径并填充到 config 对象中
    std::filesystem::path exe_path = argv[0];
    config.db_path = (exe_path.parent_path() / "workouts.sqlite3").string();
    config.mapping_path = "mapping.json";

    return config;
}

int main(int argc, char* argv[]) {
    // 1. 解析命令行，获取配置信息
    auto configOpt = parseCommandLine(argc, argv);
    if (!configOpt.has_value()) {
        // 如果返回空，说明是请求帮助或解析失败，根据错误码退出
        for (int i = 1; i < argc; ++i) {
            if (std::string(argv[i]) == "-h" || std::string(argv[i]) == "--help") return 0; // 请求帮助是正常退出
        }
        return 1; // 解析失败是错误退出
    }

    // 2. 创建核心处理器
    ActionHandler handler;

    // 3. 传入配置对象，运行主流程
    bool success = handler.run(configOpt.value());

    // 4. 根据结果返回状态码
    return success ? 0 : 1;
}