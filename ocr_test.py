from PIL import Image
import easyocr
import numpy as np
import time # 引入时间库，让我们看看有多快

# --- 配置区 ---
IMAGE_FILE_PATH = 'running_data.jpg'
CROP_BOX = (82, 935, 358, 1076) 

# --- 主程序 ---

# ====> 关键修改在这里！ <====
# 在创建 Reader 时，加入 gpu=True 参数
print("正在加载 EasyOCR 模型到 GPU...")
reader = easyocr.Reader(['ch_sim', 'en'], gpu=True) 
print("模型加载完毕。")


def extract_text_from_area(image_path, box):
    try:
        with Image.open(image_path) as img:
            print(f"图片加载成功: '{image_path}', 尺寸: {img.size}")
            cropped_image_pil = img.crop(box)
            cropped_image_np = np.array(cropped_image_pil)
            
            # 记录识别前的时间
            start_time = time.time()
            
            # 使用 EasyOCR 进行文字识别
            results = reader.readtext(cropped_image_np)
            
            # 记录识别后的时间
            end_time = time.time()
            
            extracted_texts = [text for (bbox, text, prob) in results]
            final_text = ' '.join(extracted_texts) 

            print("\n" + "="*30)
            print(f"在坐标区域 {box} 中识别到的文字内容是:")
            print("---")
            print(final_text.strip())
            print("---")
            # 计算并打印识别耗时
            print(f"本次识别耗时: { (end_time - start_time) * 1000:.2f} 毫秒")
            print("="*30)

    except Exception as e:
        print(f"程序发生错误: {e}")

# --- 运行测试 ---
if __name__ == "__main__":
    extract_text_from_area(IMAGE_FILE_PATH, CROP_BOX)