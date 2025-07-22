#ifndef ACTION_HANDLER_H
#define ACTION_HANDLER_H

#include "reprocessor/Reprocessor.h"
#include "db_inserter/DataManager.h"
#include <string>
#include <optional>

/**
 * @brief 应用程序的通用配置信息。
 *
 * 这个结构体包含了执行一次操作所需的所有参数，
 * 无论这些参数来自命令行还是交互式输入。
 */
struct AppConfig {
    std::string log_filepath;
    std::string db_path;
    std::string mapping_path;
    std::optional<int> specified_year;
};

/**
 * @brief 负责执行程序所有核心操作的顶级控制器。
 *
 * 该类接收一个配置对象并执行文件处理和数据持久化的流程。
 * 它不关心配置信息从何而来。
 */
class ActionHandler {
public:
    /**
     * @brief 根据提供的配置执行主流程。
     * @param config 包含所有操作参数的配置对象。
     * @return 成功返回 true，失败返回 false。
     */
    bool run(const AppConfig& config);

private:
    /**
     * @brief 将字符串内容写入文件。
     */
    bool writeStringToFile(const std::string& filepath, const std::string& content);

    // Handler 持有核心服务类的实例
    Reprocessor reprocessor_;
    DataManager dataManager_;
};

#endif // ACTION_HANDLER_H