# test_cleanup.py

import os
import shutil

# ===================================================================
# 配置与样式
# ===================================================================

# 定义需要清理的项
DIRS_TO_DELETE = [
    'test_output',
    'reprocessed_json',
    'output_file'
]
FILES_TO_DELETE = [
    'workout_tracker_cli.exe', 
    'mapping.json',
    'workout_logs.sqlite3' 
]

# 定义ANSI颜色代码
GREEN = '\033[92m'
RED = '\033[91m'
CYAN = '\033[96m'
RESET = '\033[0m'

# ===================================================================
#核心功能
# ===================================================================

def cleanup(base_dir):
    """
    清理上一次运行留下的文件和文件夹。
    
    :param base_dir: 脚本所在的根目录。
    :return: 如果清理成功则返回 True，否则返回 False。
    """
    print(f"{CYAN}--- 1. Cleaning Workspace ---{RESET}")
    success = True
    # 删除指定目录
    for dir_name in DIRS_TO_DELETE:
        dir_path = os.path.join(base_dir, dir_name)
        if os.path.exists(dir_path):
            try:
                shutil.rmtree(dir_path)
                print(f"  {GREEN}已删除旧的目录: {dir_name}{RESET}")
            except OSError as e:
                print(f"  {RED}删除目录 '{dir_path}' 时出错: {e}{RESET}")
                success = False
                
    # 删除指定文件
    for file_name in FILES_TO_DELETE:
        file_path = os.path.join(base_dir, file_name)
        if os.path.exists(file_path):
            try:
                os.remove(file_path)
                print(f"  {GREEN}已删除旧的文件: {file_name}{RESET}")
            except OSError as e:
                print(f"  {RED}删除文件 '{file_path}' 时出错: {e}{RESET}")
                success = False
    
    if success:
        print(f"  {GREEN}工作区清理完毕。{RESET}")
    else:
        print(f"  {RED}清理过程中发生错误。{RESET}")
        
    return success

# ===================================================================
# 独立执行入口
# ===================================================================

if __name__ == '__main__':
    """允许此脚本被独立执行以进行清理。"""
    print("正在独立执行清理操作...")
    script_dir = os.path.dirname(os.path.abspath(__file__))
    if cleanup(script_dir):
        print("\n独立清理成功完成。")
    else:
        print("\n独立清理失败。")