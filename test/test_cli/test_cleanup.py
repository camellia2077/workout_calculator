# test_cleanup.py

import os
import shutil
import config  # <<< [MODIFIED] 导入新的配置文件

# 定义ANSI颜色代码
GREEN = '\033[92m'
RED = '\033[91m'
CYAN = '\033[96m'
RESET = '\033[0m'

def cleanup(work_dir):
    """
    清理上一次运行留下的文件和文件夹。
    
    :param work_dir: 要进行清理的工作目录。
    :return: 如果清理成功则返回 True，否则返回 False。
    """
    print(f"{CYAN}--- 1. Cleaning Workspace in '{work_dir}' ---{RESET}")
    success = True
    
    # [MODIFIED] 从 config 模块读取要删除的目录
    for dir_name in config.DIRS_TO_DELETE:
        dir_path = os.path.join(work_dir, dir_name)
        if os.path.exists(dir_path):
            try:
                shutil.rmtree(dir_path)
                print(f"  {GREEN}已删除旧的目录: {dir_path}{RESET}")
            except OSError as e:
                print(f"  {RED}删除目录 '{dir_path}' 时出错: {e}{RESET}")
                success = False
                
    # [MODIFIED] 从 config 模块读取要删除的文件
    for file_name in config.FILES_TO_DELETE:
        file_path = os.path.join(work_dir, file_name)
        if os.path.exists(file_path):
            try:
                os.remove(file_path)
                print(f"  {GREEN}已删除旧的文件: {file_path}{RESET}")
            except OSError as e:
                print(f"  {RED}删除文件 '{file_path}' 时出错: {e}{RESET}")
                success = False
    
    if success:
        print(f"  {GREEN}工作区清理完毕。{RESET}")
    else:
        print(f"  {RED}清理过程中发生错误。{RESET}")
        
    return success

if __name__ == '__main__':
    """允许此脚本被独立执行以进行清理。"""
    print("正在独立执行清理操作...")
    # 确保独立运行时也能找到配置文件
    base_dir = os.path.dirname(os.path.abspath(__file__))
    if config.TEST_OUTPUT_DIR:
        cleanup_dir = config.TEST_OUTPUT_DIR
    else:
        cleanup_dir = base_dir
    
    if cleanup(cleanup_dir):
        print("\n独立清理成功完成。")
    else:
        print("\n独立清理失败。")