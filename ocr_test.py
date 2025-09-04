import easyocr
import json
from PIL import Image
import numpy as np
import torch

# --- 1. 配置区 ---

IMAGE_FILE_PATH = 'running_data.jpg'

# 将变量重命名为里程数字部分对应的英文，并使用您验证过的正确坐标
CROP_BOX_MILEAGE = (91, 943, 357, 1077) 

# --- 2. 主程序 ---

def extract_mileage_to_json(image_path, box):
    """
    从指定区域提取里程数据，并以JSON格式输出。
    """
    try:
        # 检查GPU并初始化EasyOCR Reader
        use_gpu = torch.cuda.is_available()
        if use_gpu:
            print("检测到NVIDIA GPU，将使用 GPU 加速。")
        else:
            print("警告: 未检测到GPU，将使用 CPU。")
            
        reader = easyocr.Reader(['ch_sim', 'en'], gpu=use_gpu)
        print("模型加载完毕。")

        # 打开并裁剪图片
        with Image.open(image_path) as img:
            print(f"图片 '{image_path}' 加载成功。")
            cropped_image_pil = img.crop(box)
            
            # 将图片转换为EasyOCR需要的格式
            cropped_image_np = np.array(cropped_image_pil)
            
            # 执行文字识别
            results = reader.readtext(cropped_image_np)
            
            # 清理和拼接识别结果
            # 注意：对于这个特定区域，识别结果可能包含数字和单位，我们先完整提取
            extracted_text = ' '.join([res[1] for res in results]).strip()

            # 创建一个字典来存储结果
            output_dict = {
                "mileage": extracted_text
            }
            
            # 将字典转换为格式化的JSON字符串
            # indent=4 美化输出, ensure_ascii=False 保证中文正常显示
            json_output = json.dumps(output_dict, indent=4, ensure_ascii=False)
            
            # 打印最终的JSON结果
            print("\n" + "="*30)
            print("提取完成，生成的JSON对象如下:")
            print("="*30)
            print(json_output)

    except FileNotFoundError:
        error_json = json.dumps({"error": f"图片文件未找到: {image_path}"}, indent=4)
        print(error_json)
    except Exception as e:
        error_json = json.dumps({"error": f"处理过程中发生错误: {str(e)}"}, indent=4)
        print(error_json)

# --- 3. 运行 ---
if __name__ == "__main__":
    extract_mileage_to_json(IMAGE_FILE_PATH, CROP_BOX_MILEAGE)