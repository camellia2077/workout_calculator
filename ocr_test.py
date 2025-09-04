import easyocr
import json
from PIL import Image
import numpy as np
import torch

class OcrExtractor:
    """
    A class to extract text from specified regions of an image using EasyOCR.
    
    The OCR model is initialized once when an instance of the class is created,
    making subsequent extractions much faster.
    """
    def __init__(self):
        """
        Initializes the OcrExtractor by loading the EasyOCR model.
        It automatically detects and uses a GPU if available.
        """
        use_gpu = torch.cuda.is_available()
        if use_gpu:
            print(f"检测到NVIDIA GPU: {torch.cuda.get_device_name(0)}。将使用 GPU 加速。")
        else:
            print("警告: 未检测到可用的NVIDIA GPU。将使用 CPU 运行。")
        
        # Load the model into memory. This is the slow part and only happens once.
        self.reader = easyocr.Reader(['ch_sim', 'en'], gpu=use_gpu)
        print("模型加载完毕，提取器已准备就绪。")

    def extract_data(self, image_path: str, fields: list) -> str:
        """
        Extracts data from an image based on a list of fields and their coordinates.

        :param image_path: The path to the image file.
        :param fields: A list of dictionaries. Each dictionary must contain:
                       - 'name': A string for the JSON key (e.g., "mileage").
                       - 'box': A tuple of coordinates (left, top, right, bottom).
        :return: A JSON formatted string containing the extracted data.
        """
        results_dict = {}
        try:
            # Open the source image once
            source_image = Image.open(image_path)
            print(f"\n图片 '{image_path}' 加载成功。")

            print("开始从以下字段提取数据:")
            # Loop through each field defined in the list
            for field in fields:
                field_name = field['name']
                box = field['box']
                
                print(f"  - 正在处理字段: '{field_name}'...")
                
                # Crop the image to the specified box
                cropped_image_pil = source_image.crop(box)
                
                # Convert to NumPy array for EasyOCR
                cropped_image_np = np.array(cropped_image_pil)
                
                # Perform OCR
                ocr_results = self.reader.readtext(cropped_image_np)
                
                # Join results and clean up the text
                extracted_text = ' '.join([res[1] for res in ocr_results]).strip()
                
                # Store the result in our dictionary
                results_dict[field_name] = extracted_text
            
            # Convert the final dictionary to a nicely formatted JSON string
            return json.dumps(results_dict, indent=4, ensure_ascii=False)

        except FileNotFoundError:
            return json.dumps({"error": f"图片文件未找到: {image_path}"}, indent=4, ensure_ascii=False)
        except Exception as e:
            return json.dumps({"error": f"处理过程中发生错误: {str(e)}"}, indent=4, ensure_ascii=False)

# --- 主程序运行区 ---
if __name__ == "__main__":
    
    # --- 1. 配置您想要提取的字段 ---
    # 在这个 list 中，为您想提取的每个数据点添加一个 dictionary。
    # 'name' 将是JSON中的键名。
    # 'box' 是您测量的 (left, top, right, bottom) 坐标。
    
    fields_to_extract = [
        {
            "name": "mileage", 
            "box": (91, 943, 357, 1077) # 您验证过的正确坐标
        },
        {
             "name": "time", 
             "box": (81, 1231, 349, 1309)
        }
        # 您可以继续在这里添加更多字段...
        # 例如 (这些坐标需要您自己去精确测量):
        # {
        #     "name": "duration", 
        #     "box": (85, 560, 350, 620)
        # },
        # {
        #     "name": "calories", 
        #     "box": (420, 560, 640, 615)
        # }
    ]

    # --- 2. 创建提取器实例并执行提取 ---
    
    # 创建实例 (这会初始化模型)
    extractor = OcrExtractor()
    
    # 调用方法，传入图片路径和字段列表
    final_json_output = extractor.extract_data('running_data.jpg', fields_to_extract)
    
    # --- 3. 打印最终结果 ---
    
    print("\n" + "="*40)
    print("提取完成，生成的JSON对象如下:")
    print("="*40)
    print(final_json_output)