# main_test.py

import os
import sys

# 从其他模块导入核心功能
from test_cleanup import cleanup
from test_runner import run_all_tests

# ===================================================================
# 全局配置
# ===================================================================

# [NEW] 新增测试输出目录配置
# -> 如果设置为一个路径 (例如 r"C:\temp\test_run_1")，所有输出将保存在那里。
# -> 如果设置为空字符串 ""，所有输出将保存在脚本当前所在的目录。
TEST_OUTPUT_DIR = r"C:\Computer\my_github\github_cpp\workout_calculator\mytest" 

# 定义ANSI颜色代码
GREEN = '\033[92m'
RED = '\033[91m'
CYAN = '\033[96m'
RESET = '\033[0m'

def main():
    """主函数，协调整个测试流程。"""
    # 在Windows上启用ANSI颜色
    if os.name == 'nt':
        os.system('color')
    
    # [MODIFIED] 移除 os.chdir()
    
    # 确定脚本文件自身所在的目录
    script_dir = os.path.dirname(os.path.abspath(__file__))
    
    # [MODIFIED] 根据 TEST_OUTPUT_DIR 决定测试的工作根目录
    if TEST_OUTPUT_DIR:
        # 如果指定了输出目录，则使用该目录
        work_dir = os.path.abspath(TEST_OUTPUT_DIR)
        os.makedirs(work_dir, exist_ok=True)
    else:
        # 否则，使用脚本所在的目录
        work_dir = script_dir

    print(f"\n{CYAN}========== 开始测试: Workout Tracker CLI =========={RESET}")
    print(f"脚本位置: {script_dir}")
    print(f"测试输出目录: {work_dir}")

    # 步骤 1: 清理工作区
    # 将计算出的工作目录传递给清理函数
    if not cleanup(work_dir):
        print(f"\n{RED}❌ 清理步骤失败，测试终止。{RESET}")
        sys.exit(1)
        
    # 步骤 2: 执行所有测试
    # 将脚本目录和工作目录都传递给测试执行器
    if not run_all_tests(script_dir, work_dir):
        print(f"\n{RED}❌ 测试流程失败，已终止。{RESET}")
        sys.exit(1)

    # 如果所有步骤都成功
    print(f"\n{GREEN}✅ 所有测试步骤成功完成!{RESET}")
    print(f"{GREEN}   请检查 '{os.path.join(work_dir, 'output_file', 'md')}' 目录以验证最终报告。{RESET}")
    sys.exit(0)

if __name__ == '__main__':
    main()