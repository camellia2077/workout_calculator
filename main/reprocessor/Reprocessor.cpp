// Reprocessor.cpp

#include "reprocessor/Reprocessor.h"
#include "common/JsonReader.h" 

#include <iostream>
#include <chrono>
#include <ctime>   
#include <iomanip>

bool Reprocessor::configure(const std::string& mappingFilePath) {
    auto jsonDataOpt = JsonReader::readFile(mappingFilePath);
    if (!jsonDataOpt) {
        std::cerr << "Error: [Reprocessor] Failed to read or parse mapping file." << std::endl;
        return false;
    }
    return mapper.loadMappings(jsonDataOpt.value());
}

std::vector<DailyData> Reprocessor::processLogFile(
    const std::string& logFilePath,
    std::optional<int> specifiedYear
) {
    // 步骤 1: 确定要使用的年份 (此逻辑保留在协调者中是合理的)
    int yearToUse;
    if (specifiedYear.has_value()) {
        yearToUse = specifiedYear.value();
    } else {
        // 如果未指定年份，自动获取当前系统年份
        const auto now = std::chrono::system_clock::now();
        const auto in_time_t = std::chrono::system_clock::to_time_t(now);
        std::tm buf;
        #ifdef _WIN32
            localtime_s(&buf, &in_time_t);
        #else
            localtime_r(&in_time_t, &buf);
        #endif
        yearToUse = 1900 + buf.tm_year;
    }

    // 步骤 2: (委托) 解析原始日志文件
    if (!parser.parseFile(logFilePath)) {
        std::cerr << "Error: [Reprocessor] Parsing log file failed." << std::endl;
        return {}; // 返回空向量表示失败
    }
    std::vector<DailyData> data = parser.getParsedData();

    // 步骤 3: (委托) 补全年份并格式化日期
    DateProcessor::completeDates(data, yearToUse);

    // 步骤 4: (委托) 计算训练容量
    VolumeCalculator::calculateVolume(data); 

    // 步骤 5: (委托) 应用名称映射
    for (auto& dailyData : data) {
        for (auto& project : dailyData.projects) {
            project.projectName = mapper.getFullName(project.projectName);
        }
    }

    // 步骤 6: 返回处理完成的数据
    return data;
}

std::string Reprocessor::formatDataToString(const std::vector<DailyData>& processedData)
{
    // 将格式化任务委托给 LogFormatter 模块
    return LogFormatter::format(processedData);
}