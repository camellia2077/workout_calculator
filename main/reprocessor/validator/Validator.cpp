#include "Validator.h"
#include <fstream>
#include <regex>
#include <iostream>

bool Validator::validate(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Error: [Validator] Could not open file " << filePath << std::endl;
        return false;
    }

    std::string line;
    int lineCounter = 0;
    bool expectingTitle = false;
    bool expectingContent = false;

    // 正则表达式定义
    const std::regex dateRegex(R"(^\d{4}$)"); // 4位数字
    const std::regex titleRegex(R"(^[a-z]+$)"); // 只有小写字母
    const std::regex contentRegex(R"(^\+\d+\s+\d+(\+\d+)*$)"); // "+数字 数字+数字..."

    while (std::getline(file, line)) {
        lineCounter++;
        // 移除字符串两端的空白字符
        line.erase(0, line.find_first_not_of(" \t\n\r"));
        line.erase(line.find_last_not_of(" \t\n\r") + 1);

        if (line.empty()) {
            continue; // 跳过空行
        }

        // 1. 验证日期行
        if (std::regex_match(line, dateRegex)) {
            expectingTitle = true; // 日期行后应该跟一个标题
            expectingContent = false;
            continue;
        }

        // 2. 验证标题行
        if (expectingTitle && std::regex_match(line, titleRegex)) {
            expectingContent = true; // 标题后应该跟内容
            expectingTitle = false;
            continue;
        }

        // 3. 验证内容行
        if (expectingContent && std::regex_match(line, contentRegex)) {
            expectingTitle = true; // 内容行后可以跟另一个标题或新的日期
            expectingContent = false;
            continue;
        }
        
        // 如果代码执行到这里，说明当前行不符合任何预期的格式
        std::cerr << "Error: [Validator] Invalid format at line " << lineCounter << ": " << line << std::endl;
        return false;
    }

    // 检查文件末尾的状态，确保每个标题都有内容
    if (expectingContent) {
        std::cerr << "Error: [Validator] File ended unexpectedly. A title is missing its content line." << std::endl;
        return false;
    }
    
    return true;
}