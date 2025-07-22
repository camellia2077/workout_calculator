#ifndef REPROCESSOR_H
#define REPROCESSOR_H

#include <string>
#include <vector>
#include <optional>

#include "reprocessor/_shared/parsed_data.h"
#include "reprocessor/log_parser/LogParser.h"
#include "reprocessor/name_mapper/ProjectNameMapper.h"

class Reprocessor {
public:
    /**
     * @brief 配置Reprocessor，加载所有必要的外部配置。
     * @param mappingFilePath 包含项目名称映射的JSON文件路径。
     * @return 如果配置成功返回true，否则返回false。
     */
    bool configure(const std::string& mappingFilePath);

    /**
     * @brief 处理一个日志文件，返回包含完整信息的结构化数据。
     * @param logFilePath 要处理的日志文件路径。
     * @param specifiedYear 可选参数，用于指定年份。如果未提供，则自动使用当前系统年份。
     * @return 包含所有处理后数据的向量。如果处理失败则返回空向量。
     */
    std::vector<DailyData> processLogFile(
        const std::string& logFilePath,
        std::optional<int> specifiedYear = std::nullopt
    );

private:
    LogParser parser;
    ProjectNameMapper mapper;
};

#endif // REPROCESSOR_H