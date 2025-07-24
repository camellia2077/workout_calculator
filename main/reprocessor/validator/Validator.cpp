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
    // 状态机变量：
    // expectingDate: 是否期望一个日期行。程序开始时为 true。
    // expectingTitleOrDate: 是否期望一个标题行或新的日期行。
    bool expectingDate = true;
    bool expectingTitleOrDate = false;

    // 正则表达式定义 (保持不变)
    const std::regex dateRegex(R"(^\d{4}$)");
    const std::regex titleRegex(R"(^[a-z]+$)");
    const std::regex contentRegex(R"(^\+\d+\s+\d+(\+\d+)*$)");

    while (std::getline(file, line)) {
        lineCounter++;
        // 移除字符串两端的空白字符
        line.erase(0, line.find_first_not_of(" \t\n\r"));
        line.erase(line.find_last_not_of(" \t\n\r") + 1);

        if (line.empty()) {
            continue; // 跳过空行
        }

        // --- 新的、更详细的验证逻辑 ---

        // 状态1: 期望一个日期行
        if (expectingDate) {
            if (std::regex_match(line, dateRegex)) {
                expectingDate = false;
                expectingTitleOrDate = true; // 日期之后，必须跟一个标题
                continue;
            } else {
                std::cerr << "Error: [Validator] Invalid format at line " << lineCounter
                          << ". Expected a date (e.g., '0704'), but found: \"" << line << "\"" << std::endl;
                return false;
            }
        }

        // 状态2: 期望一个标题行或新的日期行
        if (expectingTitleOrDate) {
            // 优先检查是否是新的日期，这表示上一个日期的结束
            if (std::regex_match(line, dateRegex)) {
                expectingDate = false;
                expectingTitleOrDate = true; // 重置状态，新日期后必须跟标题
                continue;
            }
            // 接着检查是否是标题
            if (std::regex_match(line, titleRegex)) {
                expectingTitleOrDate = false; // 标题之后，必须跟内容
                continue;
            }
            // 如果上述条件都不满足，说明格式错误
            else {
                std::cerr << "Error: [Validator] Invalid format at line " << lineCounter
                          << ". Expected a title (e.g., 'bp') or a new date, but found: \"" << line << "\"" << std::endl;
                return false;
            }
        }
        
        // 状态3: (默认) 期望一个内容行
        if (std::regex_match(line, contentRegex)) {
            expectingTitleOrDate = true; // 内容行之后，可以跟新标题或新日期
            continue;
        }

        // 如果代码执行到这里，说明当前行不符合任何预期的格式
        // 此时最可能的情况是期望内容行，但实际不是
        std::cerr << "Error: [Validator] Invalid format at line " << lineCounter
                  << ". Expected a content line (e.g., '+60 10+10'), but found: \"" << line << "\"" << std::endl;
        return false;
    }

    // 文件末尾检查: 如果最后期望的是一个标题或日期，说明某个标题缺少了对应的内容行
    if (expectingTitleOrDate && !expectingDate) {
         std::cerr << "Error: [Validator] File ended unexpectedly. The last title is missing its content line." << std::endl;
         return false;
    }
    
    return true;
}