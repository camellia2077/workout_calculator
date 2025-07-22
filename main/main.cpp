#include <iostream>
#include <vector>
#include <string>
#include <optional>
#include <fstream>      // 用於文件流操作
#include <filesystem>   // 用於創建目錄和處理路徑 (C++17)

// 包含總控類和共享數據結構的頭文件
#include "reprocessor/Reprocessor.h"
#include "reprocessor/_shared/parsed_data.h"

// 新函數：將處理後的數據寫入文件
// 參數: output_filepath - 完整的輸出文件路徑
// 參數: allData - 包含所有已處理數據的 vector
// 返回: 如果寫入成功返回 true，否則返回 false
bool writeFinalDataToFile(const std::string& output_filepath, const std::vector<DailyData>& allData) {
    // *** 錯誤已修正 ***
    std::ofstream file(output_filepath); // 創建並打開一個輸出文件流
    if (!file.is_open()) {
        std::cerr << "Error: Failed to open output file: " << output_filepath << std::endl;
        return false;
    }

    file << "--- Processed Workout Data ---" << std::endl;
    for (const auto& daily : allData) {
        file << "Date: " << daily.date << std::endl;
        for (const auto& project : daily.projects) {
            file << "  Project: " << project.projectName << std::endl;
            file << "    Weight: " << project.weight << "kg" << std::endl;
            file << "    Reps: ";
            for (size_t i = 0; i < project.reps.size(); ++i) {
                file << project.reps[i] << (i == project.reps.size() - 1 ? "" : ", ");
            }
            file << std::endl;
            file << "    Volume: " << project.volume << " kg" << std::endl;
        }
    }
    file << "------------------------------" << std::endl;
    file.close();
    return true;
}

// 打印使用說明
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
    std::vector<DailyData> finalData = reprocessor.processLogFile(log_filepath, specified_year);

    if (finalData.empty()) {
        std::cerr << "Error: Failed to process log file or no data found." << std::endl;
        return 1;
    }
    std::cout << "Log file processed successfully." << std::endl;

    // 4. 將最終結果寫入文件而不是打印到控制台
    try {
        const std::string output_dir = "reprocessed";
        // 創建 'reprocessed' 目錄 (如果不存在)
        std::filesystem::create_directory(output_dir);

        // 從輸入路徑構建輸出文件名
        std::filesystem::path input_path(log_filepath);
        // 確保文件名是有效的UTF-8字符串
        std::string output_filename = input_path.stem().u8string() + "_reprocessed.txt";
        std::filesystem::path output_filepath = std::filesystem::path(output_dir) / output_filename;

        std::cout << "Writing processed data to '" << output_filepath.string() << "'..." << std::endl;

        if (writeFinalDataToFile(output_filepath.string(), finalData)) {
            std::cout << "Successfully wrote data to file." << std::endl;
        } else {
            // writeFinalDataToFile 內部會打印詳細錯誤
            std::cerr << "Error: Could not write the output file." << std::endl;
            return 1;
        }
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Filesystem error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}