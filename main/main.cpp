#include <iostream>
#include <vector>
#include <fstream>
#include "reprocessor/log_parser/log_parser.h"
#include "reprocessor/name_mapper/name_mapper.h"
#include "reprocessor/_shared/parsed_data.h"
#include "reprocessor/data_processor/DataProcessor.h"

// 函数：用于打印最终结果
void printFinalData(const std::vector<DailyData>& allData) {
    std::cout << "--- Processed Workout Data ---" << std::endl;
    for (const auto& daily : allData) {
        std::cout << "Date: " << daily.date << std::endl;
        for (const auto& project : daily.projects) {
            std::cout << "  Project: " << project.projectName << std::endl;
            std::cout << "    Weight: " << project.weight << "kg" << std::endl;
            std::cout << "    Reps: ";
            for (size_t i = 0; i < project.reps.size(); ++i) {
                std::cout << project.reps[i] << (i == project.reps.size() - 1 ? "" : ", ");
            }
            std::cout << std::endl;
            // 新增：打印容量
            std::cout << "    Volume: " << project.volume << " kg" << std::endl;
        }
    }
    std::cout << "------------------------------" << std::endl;
}

int main() {
    // 1. 定义文件名和硬编码的训练数据
    const std::string log_filename = "temp_workout_log.txt"; //
    const std::string mapping_filename = "mapping.json"; //
    const std::string workout_data =
        "0721\n"
        "bp\n"
        "+60    10+10+9+8\n"
        "bbp\n"
        "+30   5+5+4\n"; //

    // 2. 将硬编码数据写入临时文件
    std::ofstream temp_log_file(log_filename); //
    if (!temp_log_file.is_open()) { //
        std::cerr << "Failed to create temporary log file." << std::endl; //
        return 1; //
    }
    temp_log_file << workout_data; //
    temp_log_file.close(); //

    // 3. 初始化映射器并加载JSON文件
    ProjectNameMapper mapper; //
    std::cout << "Loading mappings from '" << mapping_filename << "'..." << std::endl; //
    if (!mapper.loadMappings(mapping_filename)) { //
        std::cerr << "Could not load mappings. Please ensure '" << mapping_filename << "' exists and is valid." << std::endl; //
        remove(log_filename.c_str()); //
        return 1; //
    }
    std::cout << "Mappings loaded successfully." << std::endl; //

    // 4. 初始化解析器并解析日志文件
    LogParser parser; //
    std::cout << "Parsing log file '" << log_filename << "'..." << std::endl; //
    if (!parser.parseFile(log_filename)) { //
        std::cerr << "Failed to parse log file." << std::endl; //
        remove(log_filename.c_str()); //
        return 1; //
    }
    std::cout << "Log parsed successfully." << std::endl; //

    // 5. 获取一个可修改的数据副本
    std::vector<DailyData> processedData = parser.getParsedData(); //

    // 6. ---- 数据处理流程 ----
    // 步骤 6a: 计算容量
    DataProcessor::calculateVolume(processedData);

    // 步骤 6b: 应用名称映射
    for (auto& dailyData : processedData) { //
        for (auto& project : dailyData.projects) { //
            project.projectName = mapper.getFullName(project.projectName); //
        }
    }

    // 7. 打印处理后的数据
    printFinalData(processedData); //

    // 8. 清理临时文件
    remove(log_filename.c_str()); //

    return 0; //
}