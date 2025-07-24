#ifndef VALIDATOR_H
#define VALIDATOR_H

#include <string>

class Validator {
public:
    /**
     * @brief 验证原始日志文件的格式是否符合规范。
     * @param filePath 要验证的日志文件路径。
     * @return 如果文件格式有效则返回true，否则返回false。
     */
    static bool validate(const std::string& filePath);
};

#endif // VALIDATOR_H