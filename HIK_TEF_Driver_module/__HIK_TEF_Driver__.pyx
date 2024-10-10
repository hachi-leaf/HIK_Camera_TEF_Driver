# cython: language_level=3
cimport numpy as cnp
import numpy as np
import cv2

# 声明cv::Mat与data方法
cdef extern from "opencv2/core.hpp" namespace "cv":
    cdef cppclass Mat:
        unsigned char* data

# 导入类与成员
cdef extern from "HIK_TEF_Driver.hpp":
    cdef cppclass HIK_VideoCapture:
            int nWidth, nHeight, Photograph_ret
            HIK_VideoCapture() except +
            int Init_Video() except +
            void Photograph(unsigned char*, int) except +
            void Photograph_background(unsigned char*, int) except +
            void Photograph_join() except +
            void Close_Video() except +

# 定义python接口
cdef class HIK_Video:
    cdef HIK_VideoCapture* c_obj
    cdef public image

    def __cinit__(self):
        self.c_obj = new HIK_VideoCapture()
        self.image = None

    def __dealloc__(self):
        del self.c_obj
        del self.image

    # 初始化方法，返回错误码与图片大小
    def Init_Video(self):
        return <int>self.c_obj.Init_Video(), <int>self.c_obj.nHeight, <int>self.c_obj.nWidth

    # 拍摄图像
    def Photograph(self, timeout = 1000):
        self.image = np.zeros((self.c_obj.nHeight, self.c_obj.nWidth), dtype=np.uint8) # 创建容器
        self.c_obj.Photograph_background(<unsigned char*>cnp.PyArray_DATA(self.image), 1000) # 获取Bayer RG格式图像

        # 将Bayer RG转为BGR图像并返回
        # return <int>ret, cv2.cvtColor(image, cv2.COLOR_BayerRG2RGB) 

    # 查询拍摄是否完成
    def isimagereturned(self):
        if <int>self.c_obj.Photograph_ret == -2:
            return False
        else:
            return True

    def get_image(self):
        # 结束线程
        self.c_obj.Photograph_join()
        # 将Bayer RG转为BGR图像并返回
        return <int>self.c_obj.Photograph_ret, cv2.cvtColor(self.image, cv2.COLOR_BayerRG2RGB)

    # 关闭摄像头
    def Close_Video(self):
        self.c_obj.Close_Video()
        