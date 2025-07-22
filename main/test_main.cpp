#include <iostream>
#include <fstream> // 用于文件操作
#include <vector>
#include "parser.h" // 包含解析器的声明

// 函数：用于打印解析后的数据，方便验证
void printResult(const DataParser& parser) {
    const std::vector<DailyData>& allData = parser.getParsedData();

    if (allData.empty()) {
        std::cout << "No data was parsed." << std::endl;
        return;
    }

    std::cout << "--- Parsed Content ---" << std::endl;
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
        }
    }
    std::cout << "----------------------" << std::endl;
}

int main() {
    // 1. 准备测试数据和文件名
    const std::string filename = "test_log.txt";
    const std::string test_data = 
        "0721\n"
        "bp\n"
        "+60    10+10+9+8\n"
        "squat\n"
        "+100   5+5+5\n";

    // 2. 将测试数据写入临时文件
    std::ofstream test_file(filename);
    if (!test_file.is_open()) {
        std::cerr << "Failed to create temporary test file." << std::endl;
        return 1;
    }
    test_file << test_data;
    test_file.close();

    // 3. 创建解析器实例并进行解析
    DataParser parser;
    std::cout << "Parsing file: " << filename << std::endl;
    bool success = parser.parseFile(filename);

    // 4. 检查解析是否成功并打印结果
    if (success) {
        std::cout << "File parsed successfully." << std::endl;
        printResult(parser);
    } else {
        std::cerr << "Failed to parse the file." << std::endl;
    }

    // 5. 清理：删除临时文件
    remove(filename.c_str());

    return 0;
}