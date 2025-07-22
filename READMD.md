# fitness_calculator
一个用于处理健身数据的程序

# 目录结构
```
.
├── build.sh
├── CMakeLists.txt
├── main.cpp
├── mapping.json
├── common/
│   ├── JsonReader.cpp
│   ├── JsonReader.h
│   └── parsed_data.h # 共享的log结构体数据类型
├── db_inserter/
│   ├── SqliteManager.cpp
│   └── SqliteManager.h
└── reprocessor # 数据预处理
    ├── date_processor # 补全日期
    │   ├── DateProcessor.cpp
    │   └── DateProcessor.h
    ├── log_formatter # 把解析的内容格式化输出
    │   ├── LogFormatter.cpp
    │   └── LogFormatter.h
    ├── log_parser # 把log内容解析成结构体 
    │   ├── LogParser.cpp
    │   └── LogParser.h
    ├── name_mapper # 映射项目名称
    │   ├── ProjectNameMapper.cpp
    │   └── ProjectNameMapper.h
    └── volume_calculator/ 计算容量
    │    ├── VolumeCalculator.cpp
    │    └── VolumeCalculator.h
    ├── Reprocessor.cpp # 封装
    └── Reprocessor.h
 
```