import cv2, time, os
from tqdm import tqdm
from HIK_TEF_Driver_module import HIK_TEF_Driver

# 创建摄像头对象
Video = HIK_TEF_Driver.HIK_Video()
ret, h, w = Video.Init_Video()
print("Image's height {} width {}.".format(h,w))

print("Photograph Begin...")
begin_time = time.time()
# 循环拍摄300张
for i in tqdm(range(300)):
    Video.Photograph() # 拍摄启动
    while not Video.isimagereturned(): # 等待拍摄完成
        pass
    # 记录第一次拍摄和最后一次拍摄的结果
    if i == 0:
        ret_first, image_first = Video.get_image()
    else:
        ret_last, image_last = Video.get_image()

print("frame rate: {} fps".format(300/(time.time()-begin_time)))

if ret_first != 0 and ret_last != 0: # 错误返回
    Video.Close_Video()
    print("Error {}/{}".format(ret_first, ret_last))
    exit()
else: # 保存图像
    cv2.imwrite('test_first.png', image_first)
    cv2.imwrite('test_last.png', image_last)
    print('Images saved.')

Video.Close_Video()