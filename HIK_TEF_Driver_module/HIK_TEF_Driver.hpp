# ifndef HIK_TEF_DRIVER_HPP
# define HIK_TEF_DRIVER_HPP

# include <stdio.h>
# include <iostream>
# include <opencv2/opencv.hpp>
# include <cstring>
# include <thread>

// HIK SDK
# include "include/MvCameraControl.h"
# include "include/CameraParams.h"
# include "include/MvErrorDefine.h"
# include "include/MvISPErrorDefine.h"
# include "include/PixelType.h"

class HIK_VideoCapture{
    private:
        int nRet = MV_OK; // ret变量
        void* handle; // 相机句柄
        MV_CC_DEVICE_INFO_LIST stDeviceList; // 设备列表
        MVCC_INTVALUE stParam; // 照相参数
        MV_FRAME_OUT_INFO_EX stImageInfo; // 图像信息结构体
        std::thread background; // 声明后台线程

    public:
        int nWidth=0, nHeight=0; // 图片大小
        int Photograph_ret = 0; // 拍摄函数的错误码，0为无错误
        // 构造函数
        HIK_VideoCapture(void);
        // 初始化
        int Init_Video(void);
        // 获取图像并覆盖pData
        void Photograph(unsigned char* pData, const int timeout);
        // Photograph的多线程封装
        void Photograph_background(unsigned char* pData, const int timeout);
        // 等待拍摄完成
        void Photograph_join();
        // 关闭相机
        void Close_Video(void);
};

# endif