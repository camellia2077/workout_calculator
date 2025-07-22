#include "name_mapper.h"
#include <fstream>
#include <iostream>

// 您需要确保您的编译环境可以找到这个json库的头文件
// 例如，将其放在项目目录或系统包含路径中
#include "nlohmann/json.hpp"

// 为了方便，我们直接使用nlohmann/json的命名空间
using json = nlohmann::json;

bool ProjectNameMapper::loadMappings(const std::string& jsonFilePath) {
    std::ifstream jsonFile(jsonFilePath);
    if (!jsonFile.is_open()) {
        std::cerr << "Error: Could not open mapping file " << jsonFilePath << std::endl;
        return false;
    }

    try {
        json mappings_json;
        jsonFile >> mappings_json; // 从文件解析JSON

        // 遍历JSON对象并填充我们的map
        for (auto& el : mappings_json.items()) {
            if (el.value().is_string()) {
                mappings[el.key()] = el.value();
            }
        }
    } catch (json::parse_error& e) {
        std::cerr << "Error: JSON parsing failed in file " << jsonFilePath << ": " << e.what() << std::endl;
        return false;
    }

    return true;
}

std::string ProjectNameMapper::getFullName(const std::string& shortName) const {
    auto it = mappings.find(shortName);
    if (it != mappings.end()) {
        // 如果找到了映射，返回全名
        return it->second;
    }
    // 如果没找到，返回原始名称
    return shortName;
}