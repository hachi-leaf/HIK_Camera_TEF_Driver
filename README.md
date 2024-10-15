# HIK_Camera_TEF_Driver
使用`C++`多线程驱动海康工业相机，并使用`Cython`将函数接口转为`Python`可用的链接共享库。  
关于本项目的任何问题可以邮箱联系我：zxy_yys_leaf@163.com  

## 使用方法
### 前置条件
- 主控设备：本项目适用于aarch64/arm64架构的嵌入式Linux设备，在`RDK X5`中完成开发，欲使用PC机测试相机功能请使用海康官方MVS上位机程序，欲使用其他架构的嵌入式设备需要自行修改项目。
- 相机设备：本项目使用海康工业相机，型号为`MV-CS016-10UC USB彩色`，使用USB连接主控板，遵循`USB 3.0 Vision`协议，欲使用其他型号相机需要保证相机的像素格式可以调整为`Bayer RG 8`且同样遵循`USB 3.0 Vision`协议。
- 使用前需要在MVS上位机软件中调试好相机的曝光时间、数字补偿、模拟补偿、亮度等参数，将相机的像素格式调整为`Bayer RG 8`（其他型号相机同理），欲使用其他像素格式需要自行修改项目，调整完成后设置为启动默认参数。
### 编译`Python`可用的链接共享库
进入目录`HIK_TEF_Driver_module`，键入`sudo python3 setup.py build_ext --inplace`进行编译，若弹出有关`Numpy API`的警告可忽略，终端输出绿色字样`Setup has been completed!`即表示编译成功，此时目录`HIK_TEF_Driver_module`下会生成编译文件`__HIK_TEF_Driver__.cpp`、链接共享库文件`HIK_TEF_Driver.cpython-310-aarch64-linux-gnu.so`和`build`目录。  
在项目目录下的python脚本中，可以以下语句引入相机驱动库。  
```python
from HIK_TEF_Driver_module import HIK_TEF_Driver
```
欲在其他路径文件中引入`HIK_TEF_Driver`，请将`HIK_TEF_Driver_module`目录复制到相应路径下或加入环境路径。  
欲在`sudo`环境下引入`HIK_TEF_Driver`还需要将`HIK_TEF_Driver_module/lib`目录与`HIK_TEF_Driver_module/include`目录加入`root`环境路径。  
### 运行示例测试程序
完成链接共享库的编译后，在项目目录下键入：  
```shell
python3 test_HIK.py
```
以运行相机测试示例程序。  
程序运行后，会在终端中打印输出图像的尺寸，随后打印输出`Photograph Begin...`，然后显示进度条，连续拍摄300张图片，然后计算拍摄帧率并打印输出到终端，最后将拍摄的第一张图像与最后一张图像存储为`test_first.png`和`test_last.png`。
## 文件结构
- `HIK_TEF_Driver_module/include`：
SDK头文件目录。  
- `HIK_TEF_Driver_module/lib`：
SDK库文件目录。  
- `HIK_TEF_Driver_module/HIK_TEF_Driver.hpp`：
相机调用程序头文件。  
- `HIK_TEF_Driver_module/HIK_TEF_Driver.cpp`：
相机调用程序库文件。  
- `HIK_TEF_Driver_module/__HIK_TEF_Driver__.pyx`：
`Cython`接口类型转换文件。  
- `HIK_TEF_Driver_module/setup.py`：
链接共享库编译文件。  
- `test_HIK.py`：
相机Python接口测试示例程序。  
## 源代码解释
在此解释`__HIK_TEF_Driver__.pyx`文件中定义的类与方法。  
### 私有成员、构造函数与析构函数
`HIK_Video`类的初始化无需传入参数。`HIK_Video`类的成员`c_obj`用于与`C++`类交互，`image`是储存图像的容器。`HIK_Video`类实例化时，成员的`c_obj`类也随之实例化，成员`image`被赋予`None`值。类销毁时`c_obj`和`image`也随之销毁。  
```python
cdef class HIK_Video:
    cdef HIK_VideoCapture* c_obj
    cdef public image

    def __cinit__(self):
        self.c_obj = new HIK_VideoCapture()
        self.image = None

    def __dealloc__(self):
        del self.c_obj
        del self.image
```
### 初始化并启动相机`Init_Video()`
该方法调用`C++`类`HIK_VideoCapture()`的`Init_Video()`方法并返回`HIK_VideoCapture()`的`nHeight`和`nWidth`成员，并转换为`python`的`int`类型返回。  
- `<int>self.c_obj.Init_Video()`：错误码  
`0`：无错误  
`-1`：获取设备列表失败  
`-2`：无设备连接  
`-3`：句柄创建失败  
`-4`：设备启动失败  
`-5`：Width获取失败  
`-6`：Height获取失败  
- `<int>self.c_obj.nHeight`：图像高度
- `<int>self.c_obj.nWidth`：图像宽度
```python
    def Init_Video(self):
        return <int>self.c_obj.Init_Video(), <int>self.c_obj.nHeight, <int>self.c_obj.nWidth
```
### 拍摄图像`Photograph(timeout = 1000)`
该方法为`image`成员分配空间，并调用`C++`类`HIK_VideoCapture()`的`Photograph_background()`方法创建线程后从相机读取图像并在后台线程等待数据传输，无返回值。  
- `timeout`：最大等待时间，至少需要大于`1/相机帧率`，单位`毫秒`，默认为值为`1000`。  
```python
    def Photograph(self, timeout = 1000):
        self.image = np.zeros((self.c_obj.nHeight, self.c_obj.nWidth), dtype=np.uint8)
        self.c_obj.Photograph_background(<unsigned char*>cnp.PyArray_DATA(self.image), 1000)
```
### 查询拍摄是否完成`isimagereturned(self)`：
该方法查询`C++`类`HIK_VideoCapture()`的成员`hotograph_ret`查询拍摄是否完成，返回布尔值。  
```python
    # 查询拍摄是否完成
    def isimagereturned(self):
        if <int>self.c_obj.Photograph_ret == -2:
            return False
        else:
            return True
```
### 获取图像`get_image(self)`
使用该方法前务必保证`Photograph(timeout = 1000)`方法至少被调用过一次，该方法会调用`C++`类`HIK_VideoCapture()`的`Photograph_join()`方法等待并关闭后台拍摄线程。查询`C++`类`HIK_VideoCapture()`的`Photograph_ret`成员并将其转换为`python`的`int`类型并返回，并将数据已被新图像覆盖的`image`成员由`Bayer RG 8`转换为`RGB`格式的`opencv-python`图像一并返回。
- `<int>self.c_obj.Photograph_ret`：错误码  
`0`：无错误  
`-1`：获取错误  
`-2`：线程未结束  
- cv2.cvtColor(self.image, cv2.COLOR_BayerRG2RGB)：`RGB`格式的`opencv-python`图像
```python
    def get_image(self):
        # 结束线程
        self.c_obj.Photograph_join()
        # 将Bayer RG转为BGR图像并返回
        return <int>self.c_obj.Photograph_ret, cv2.cvtColor(self.image, cv2.COLOR_BayerRG2RGB)
```
### 关闭摄像头`Close_Video()`
该方法会调用`C++`类`HIK_VideoCapture()`的`Close_Video()`停止取流，关闭设备并销毁设备句柄。