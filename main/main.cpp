// main.cpp

#include <iostream>
#include <vector>
#include <string>
#include <optional>
#include <fstream>
#include <filesystem>

// 包含总控类头文件
#include "reprocessor/Reprocessor.h"

/**
 * @brief 将一个字符串内容写入指定的文件。
 * 这个函数取代了之前复杂的 writeFinalDataToFile 函数。
 * @param output_filepath 完整的输出文件路径。
 * @param content 要写入文件的字符串内容。
 * @return 如果写入成功返回 true，否则返回 false。
 */
bool writeStringToFile(const std::string& output_filepath, const std::string& content) {
    std::ofstream file(output_filepath);
    if (!file.is_open()) {
        std::cerr << "Error: Failed to open output file: " << output_filepath << std::endl;
        return false;
    }
    file << content; // 直接写入由 Reprocessor 格式化好的字符串
    file.close();
    return true;
}

// 打印使用说明
void printUsage(const char* programName) {
    std::cerr << "Usage: " << programName << " --path <log_file.txt> [--year <YYYY>]" << std::endl;
    std::cerr << "Options:" << std::endl;
    std::cerr << "  -p, --path <file>    Required: Path to the workout log file." << std::endl;
    std::cerr << "  -y, --year <year>    Optional: Specify a year for processing. Defaults to the current system year." << std::endl;
}

int main(int argc, char* argv[]) {
    // 1. 解析命令行参数
    std::string log_filepath;
    std::optional<int> specified_year;
    const std::string mapping_filename = "mapping.json";

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if ((arg == "-p" || arg == "--path") && i + 1 < argc) {
            log_filepath = argv[++i];
        } else if ((arg == "-y" || arg == "--year") && i + 1 < argc) {
            try {
                specified_year = std::stoi(argv[++i]);
            } catch (const std::exception& e) {
                std::cerr << "Error: Invalid year format provided." << std::endl;
                return 1;
            }
        } else {
            std::cerr << "Error: Unknown or invalid argument '" << arg << "'" << std::endl;
            printUsage(argv[0]);
            return 1;
        }
    }

    if (log_filepath.empty()) {
        std::cerr << "Error: Log file path is required." << std::endl;
        printUsage(argv[0]);
        return 1;
    }

    // 2. 创建并配置Reprocessor实例
    Reprocessor reprocessor;
    std::cout << "Configuring reprocessor with '" << mapping_filename << "'..." << std::endl;
    if (!reprocessor.configure(mapping_filename)) {
        std::cerr << "Error: Failed to configure reprocessor." << std::endl;
        return 1;
    }
    std::cout << "Reprocessor configured successfully." << std::endl;

    // 3. 调用Reprocessor处理日志文件
    std::cout << "Processing log file '" << log_filepath << "'..." << std::endl;
    std::vector<DailyData> processedData = reprocessor.processLogFile(log_filepath, specified_year);

    if (processedData.empty()) {
        std::cerr << "Error: Failed to process log file or no data found." << std::endl;
        return 1;
    }
    std::cout << "Log file processed successfully." << std::endl;
    
    // --- 步骤 4 已更新 ---
    // 4. 调用 Reprocessor 将处理后的数据格式化为字符串
    std::cout << "Formatting data for output..." << std::endl;
    std::string outputContent = reprocessor.formatDataToString(processedData);

    // 5. 将格式化后的字符串写入文件
    try {
        const std::string output_dir = "reprocessed";
        std::filesystem::create_directory(output_dir);

        std::filesystem::path input_path(log_filepath);
        std::string output_filename = input_path.stem().u8string() + "_reprocessed.txt";
        std::filesystem::path output_filepath = std::filesystem::path(output_dir) / output_filename;

        std::cout << "Writing processed data to '" << output_filepath.string() << "'..." << std::endl;
        
        // 调用新的写入函数
        if (writeStringToFile(output_filepath.string(), outputContent)) {
            std::cout << "Successfully wrote data to file." << std::endl;
        } else {
            std::cerr << "Error: Could not write the output file." << std::endl;
            return 1;
        }
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Filesystem error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}