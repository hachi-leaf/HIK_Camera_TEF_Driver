# include "HIK_TEF_Driver.hpp"

// 构造函数
HIK_VideoCapture::HIK_VideoCapture(void){
    handle = nullptr; // 创建空句柄
}

// 初始化-启动相机
int HIK_VideoCapture::Init_Video(void){
    // 获取设备列表
    nRet = MV_CC_EnumDevices(MV_GIGE_DEVICE | MV_USB_DEVICE, &stDeviceList);
    if (nRet != MV_OK)return -1; // 获取设备列表失败 *MV_CC_EnumDevices failed
    if (stDeviceList.nDeviceNum == 0)return -2;// 无设备 *No devices found

    // 创建相机句柄
    nRet = MV_CC_CreateHandle(&handle, stDeviceList.pDeviceInfo[0]);
    if (nRet != MV_OK)return -3; // 创建失败 *MV_CC_CreateHandle failed

    // 启动相机
    nRet = MV_CC_OpenDevice(handle);
    if (nRet != MV_OK){
        MV_CC_DestroyHandle(handle); // 销毁句柄
        return -4; // 启动失败 *MV_CC_OpenDevice failed)
    }

    // 获取图像大小
    MVCC_INTVALUE stParam;
    std::memset(&stParam, 0, sizeof(MVCC_INTVALUE));
    nRet = MV_CC_GetIntValue(handle, "Width", &stParam);
    if (nRet != MV_OK) {
        MV_CC_CloseDevice(handle);
        MV_CC_DestroyHandle(handle);
        return -5; // 获取Width失败 *Get Width failed
    }
    nWidth = stParam.nCurValue;

    nRet = MV_CC_GetIntValue(handle, "Height", &stParam);
    if (nRet != MV_OK) {
        MV_CC_CloseDevice(handle);
        MV_CC_DestroyHandle(handle);
        return -6; // 获取Height失败 *Get Height failed
    }
    nHeight = stParam.nCurValue;

    // 开始取流
    nRet = MV_CC_StartGrabbing(handle);
    if (nRet != MV_OK) {
        MV_CC_CloseDevice(handle);
        MV_CC_DestroyHandle(handle);
        return -7; // 取流失败 *MV_CC_StartGrabbing failed
    }
    return 0;
}

// 获取图像并覆盖pData
void HIK_VideoCapture::Photograph(unsigned char* pData, const int timeout){
    // 初始化图片信息结构体
    stImageInfo = {0};
    std::memset(&stImageInfo, 0, sizeof(MV_FRAME_OUT_INFO_EX));

    // 获取图像
    // MV_CC_GetOneFrameTimeout()函数会清空缓冲区，也就是说一定会获取最新的图像
    nRet = MV_CC_GetOneFrameTimeout(handle, pData, nWidth * nHeight * 3, &stImageInfo, timeout);
    if (nRet != MV_OK) {
        delete[] pData;
        MV_CC_StopGrabbing(handle);
        MV_CC_CloseDevice(handle);
        MV_CC_DestroyHandle(handle);
        Photograph_ret = -1;// 获取失败 *MV_CC_GetOneFrameTimeout failed!
        return ; 
    }

    Photograph_ret = 0;// 返回正常代码，此时pData已被覆盖
    return ;
}

// Photograph的多线程封装
void HIK_VideoCapture::Photograph_background(unsigned char* pData, const int timeout){
    Photograph_ret = -2; // 线程进行中
    background = std::thread(&HIK_VideoCapture::Photograph, this, pData, timeout);
    return ;
}

// 等待拍摄完成
void HIK_VideoCapture::Photograph_join(){
    background.join();
    if(Photograph_ret == -2)Photograph_ret = 0; // 线程正常结束
    return ;
}

// 关闭相机
void HIK_VideoCapture::Close_Video(void){
    MV_CC_StopGrabbing(handle);
    MV_CC_CloseDevice(handle);
    MV_CC_DestroyHandle(handle);
}
