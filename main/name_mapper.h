#ifndef NAME_MAPPER_H
#define NAME_MAPPER_H

#include <string>
#include <map>

class ProjectNameMapper {
public:
    // 从JSON文件加载映射规则
    bool loadMappings(const std::string& jsonFilePath);

    // 获取映射后的全名。如果找不到映射，则返回原始名称。
    std::string getFullName(const std::string& shortName) const;

private:
    std::map<std::string, std::string> mappings;
};

#endif // NAME_MAPPER_H