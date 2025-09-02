# test_runner.py

import os
import shutil
import subprocess
import time
import re
import sys

# ===================================================================
# 全局配置
# ===================================================================
BUILD_DIR = r"C:\Computer\my_github\github_cpp\workout_calculator\workout_calculator\workout_calculator\build"
INPUT_DIR = r"C:\Computer\my_github\github_cpp\workout_calculator\RECORD"
EXE_NAME = 'workout_tracker_cli.exe'
CONFIG_NAME = 'mapping.json'

# 定义ANSI颜色代码
GREEN = '\033[92m'
YELLOW = '\033[93m'
RED = '\033[91m'
CYAN = '\033[96m'
RESET = '\033[0m'

# ===================================================================
# 辅助函数 (保持不变)
# ===================================================================

def _remove_ansi_codes(text):
    ansi_escape = re.compile(r'\x1B(?:[@-Z\\-_]|\[[0-?]*[ -/]*[@-~])')
    return ansi_escape.sub('', text)

def _execute_command(exe_path, args, log_filename, output_dir):
    full_command = [exe_path] + args
    log_path = os.path.join(output_dir, log_filename)
    
    # 为了确保程序能找到 mapping.json 等相对路径文件，我们总是在可执行文件所在的目录执行它
    execution_dir = os.path.dirname(exe_path)
    
    print(f"  -> 执行命令: {' '.join(full_command)}")
    print(f"     工作目录: {execution_dir}")
    print(f"     日志文件: {log_path}", end='', flush=True)

    try:
        start_time = time.time()
        result = subprocess.run(
            full_command, capture_output=True, text=True, encoding='utf-8', 
            errors='ignore', cwd=execution_dir
        )
        duration = time.time() - start_time

        with open(log_path, 'w', encoding='utf-8') as f:
            f.write(f"--- Command ---\n{' '.join(full_command)}\n\n")
            f.write(f"--- Return Code ---\n{result.returncode}\n\n")
            f.write(f"--- STDOUT ---\n{_remove_ansi_codes(result.stdout)}\n")
            if result.stderr:
                f.write(f"\n--- STDERR ---\n{_remove_ansi_codes(result.stderr)}\n")

        if result.returncode == 0:
            print(f"  ... {GREEN}OK{RESET} ({duration:.2f}s)")
            return True
        else:
            print(f"  ... {RED}FAILED{RESET} (返回码: {result.returncode})")
            print(f"      请查看日志获取详情: {log_path}")
            return False
    except Exception as e:
        print(f"  ... {RED}CRASHED{RESET}\n      错误: {e}")
        return False

# ===================================================================
# 测试任务 (将 base_dir 参数改为 work_dir)
# ===================================================================

def _prepare_executable(work_dir):
    print(f"{CYAN}--- 2. Preparing Dependencies ---{RESET}")
    files_to_copy = { EXE_NAME: "可执行文件", CONFIG_NAME: "配置文件" }
    for file_name, desc in files_to_copy.items():
        source = os.path.join(BUILD_DIR, file_name)
        dest = os.path.join(work_dir, file_name)
        if not os.path.exists(source):
            print(f"  {RED}错误: 源文件未找到 '{source}'. 请检查 'BUILD_DIR' 变量。{RESET}")
            return False
        shutil.copy(source, dest)
        print(f"  {GREEN}{desc} '{file_name}' 已成功复制到 '{work_dir}'。{RESET}")
    return True

def _run_validation_test(work_dir, output_dir):
    print(f"{CYAN}--- 3. Running Validation Test ---{RESET}")
    return _execute_command(os.path.join(work_dir, EXE_NAME), ["validate", INPUT_DIR], "validation_test.log", output_dir)

def _run_conversion_test(work_dir, output_dir):
    print(f"{CYAN}--- 4. Running Conversion Test ---{RESET}")
    return _execute_command(os.path.join(work_dir, EXE_NAME), ["convert", INPUT_DIR], "conversion_test.log", output_dir)

def _run_insertion_test(work_dir, output_dir):
    print(f"{CYAN}--- 5. Running Database Insertion Test ---{RESET}")
    json_dir = os.path.join(work_dir, 'reprocessed_json')
    if not os.path.exists(json_dir):
        print(f"  {RED}错误: 未找到 '{json_dir}' 目录。转换步骤可能已失败。{RESET}")
        return False
    return _execute_command(os.path.join(work_dir, EXE_NAME), ["insert", json_dir], "insertion_test.log", output_dir)

def _run_export_test(work_dir, output_dir):
    print(f"{CYAN}--- 6. Running Report Export Test ---{RESET}")
    return _execute_command(os.path.join(work_dir, EXE_NAME), ["export"], "export_test.log", output_dir)

# ===================================================================
# 主调用函数 (核心修改处)
# ===================================================================

# [MODIFIED] 更新函数定义，以接收两个参数
def run_all_tests(script_dir, work_dir):
    """
    按顺序执行所有测试步骤。
    
    :param script_dir: 脚本所在的目录 (保留，未来可能需要)。
    :param work_dir: 测试文件生成和执行的工作目录。
    :return: 如果所有测试成功则返回 True，否则返回 False。
    """
    # [MODIFIED] 使用 work_dir 来创建输出目录
    output_dir = os.path.join(work_dir, 'test_output')
    os.makedirs(output_dir, exist_ok=True)
    
    # [MODIFIED] 将 base_dir 全部更新为 work_dir
    test_steps = [
        {"func": _prepare_executable, "name": "准备依赖文件", "args": [work_dir]},
        {"func": _run_validation_test, "name": "验证测试", "args": [work_dir, output_dir]},
        {"func": _run_conversion_test, "name": "转换测试", "args": [work_dir, output_dir]},
        {"func": _run_insertion_test, "name": "数据库插入测试", "args": [work_dir, output_dir]},
        {"func": _run_export_test, "name": "报告导出测试", "args": [work_dir, output_dir]},
    ]
    
    for step in test_steps:
        if not step["func"](*step["args"]):
            print(f"\n{RED}❌ {step['name']}失败。请检查日志 '{output_dir}' 获取详情。{RESET}")
            return False
            
    return True