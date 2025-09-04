import easyocr
import json
from PIL import Image
import numpy as np
import torch
import config  # <--- 关键改动：在这里导入您的配置文件

class OcrExtractor:
    """
    一个从图片指定区域提取文本的类。
    模型在创建实例时仅初始化一次，以提高效率。
    """
    def __init__(self):
        """
        初始化OcrExtractor，加载EasyOCR模型。
        如果可用，会自动检测并使用GPU。
        """
        use_gpu = torch.cuda.is_available()
        if use_gpu:
            print(f"检测到NVIDIA GPU: {torch.cuda.get_device_name(0)}。将使用 GPU 加速。")
        else:
            print("警告: 未检测到可用的NVIDIA GPU。将使用 CPU 运行。")
        
        self.reader = easyocr.Reader(['ch_sim', 'en'], gpu=use_gpu)
        print("模型加载完毕，提取器已准备就绪。")

    def extract_data(self, image_path: str, fields: list) -> str:
        """
        根据字段列表和坐标从图像中提取数据。

        :param image_path: 图片文件的路径。
        :param fields: 从配置文件中加载的字段列表。
        :return: 包含提取结果的JSON格式字符串。
        """
        results_dict = {}
        try:
            source_image = Image.open(image_path)
            print(f"\n图片 '{image_path}' 加载成功。")

            print("开始从以下字段提取数据:")
            for field in fields:
                field_name = field['name']
                box = field['box']
                
                print(f"  - 正在处理字段: '{field_name}'...")
                
                cropped_image_pil = source_image.crop(box)
                cropped_image_np = np.array(cropped_image_pil)
                ocr_results = self.reader.readtext(cropped_image_np)
                extracted_text = ' '.join([res[1] for res in ocr_results]).strip()
                results_dict[field_name] = extracted_text
            
            return json.dumps(results_dict, indent=4, ensure_ascii=False)

        except FileNotFoundError:
            return json.dumps({"error": f"图片文件未找到: {image_path}"}, indent=4, ensure_ascii=False)
        except Exception as e:
            return json.dumps({"error": f"处理过程中发生错误: {str(e)}"}, indent=4, ensure_ascii=False)

# --- 主程序运行区 ---
if __name__ == "__main__":
    
    # --- 1. 配置已从 config.py 文件中自动加载 ---
    #    要修改或添加字段，请直接编辑 config.py 文件。
    
    # --- 2. 创建提取器实例并执行提取 ---
    extractor = OcrExtractor()
    
    # <--- 关键改动：使用从 config.py 导入的 FIELDS 列表 ---
    final_json_output = extractor.extract_data('running_data.jpg', config.FIELDS)
    
    # --- 3. 打印最终结果 ---
    print("\n" + "="*40)
    print("提取完成，生成的JSON对象如下:")
    print("="*40)
    print(final_json_output)