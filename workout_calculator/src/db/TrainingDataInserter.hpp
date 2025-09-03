// src/db/TrainingDataInserter.hpp

#ifndef TRAINING_DATA_INSERTER_H
#define TRAINING_DATA_INSERTER_H

#include "nlohmann/json.hpp"
#include "sqlite3.h"

/**
 * @brief 负责将训练数据的JSON对象插入数据库。
 * 此类封装了SQL语句准备、绑定和执行的具体细节，
 * 但不负责事务管理。
 */
class TrainingDataInserter {
public:
    explicit TrainingDataInserter(sqlite3* db);

    /**
     * @brief 执行插入操作。
     * @param jsonData 包含训练数据的JSON对象。
     * @return 如果成功返回true，如果发生任何错误则抛出std::runtime_error。
     */
    bool insert(const nlohmann::json& jsonData);

private:
    sqlite3* db_;
};

#endif // TRAINING_DATA_INSERTER_H