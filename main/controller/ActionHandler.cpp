#include "ActionHandler.h"
#include <iostream>
#include <fstream>
#include <filesystem>

bool ActionHandler::run(const AppConfig& config) {
    // --- 开始执行核心工作流 ---

    // 1. 配置 Reprocessor
    std::cout << "Configuring reprocessor with '" << config.mapping_path << "'..." << std::endl;
    if (!reprocessor_.configure(config.mapping_path)) { return false; }
    std::cout << "Reprocessor configured successfully." << std::endl;

    // 2. 处理日志文件
    std::cout << "Processing log file '" << config.log_filepath << "'..." << std::endl;
    std::vector<DailyData> processedData = reprocessor_.processLogFile(config.log_filepath, config.specified_year);
    if (processedData.empty()) {
        std::cout << "Warning: No data was processed from the log file. Exiting." << std::endl;
        return true; // 没有数据不是一个致命错误
    }
    std::cout << "Log file processed successfully." << std::endl;

    // 3. 将格式化后的字符串写入文件
    std::string outputContent = reprocessor_.formatDataToString(processedData);
    try {
        const std::string output_dir = "reprocessed";
        std::filesystem::create_directory(output_dir);
        std::filesystem::path input_path(config.log_filepath);
        std::string output_filename = input_path.stem().string() + "_reprocessed.txt";
        std::filesystem::path output_filepath = std::filesystem::path(output_dir) / output_filename;

        std::cout << "Writing processed data to '" << output_filepath.string() << "'..." << std::endl;
        if (!writeStringToFile(output_filepath.string(), outputContent)) { return false; }
        std::cout << "Successfully wrote data to file." << std::endl;
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Filesystem error: " << e.what() << std::endl;
        return false;
    }

    // 4. 使用 DataManager 保存数据到数据库
    std::cout << "----------------------------------------" << std::endl;
    std::cout << "Database sync process started..." << std::endl;
    if (!dataManager_.connectAndInitialize(config.db_path)) {
        std::cerr << "Error: Database setup failed for: " << config.db_path << std::endl;
        return false;
    }
    if (!dataManager_.saveData(processedData)) {
        std::cerr << "Error: Failed to save data to the database." << std::endl;
        return false;
    }
    std::cout << "Data successfully saved to database: " << config.db_path << std::endl;

    return true;
}

// 私有辅助函数保持不变
bool ActionHandler::writeStringToFile(const std::string& filepath, const std::string& content) {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Error: Failed to open output file: " << filepath << std::endl;
        return false;
    }
    file << content;
    file.close();
    return true;
}