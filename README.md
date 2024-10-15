# HIK_Camera_TEF_Driver
使用`C++`多线程驱动海康工业相机，并使用`Cython`将函数接口转为`Python`可用的链接共享库。  
## 使用方法
### 前置条件
- 主控设备：本项目适用于arrch64/arm64架构的嵌入式Linux设备，在`RDK X5`中完成开发，欲使用PC机测试相机功能请使用海康官方MVS上位机程序，欲使用其他架构的嵌入式设备需要自行修改项目。
- 相机设备：本项目使用海康工业相机，型号为`MV-CS016-10UC USB彩色`，使用USB连接主控板，遵循`USB 3.0 Vision`协议，欲使用其他型号相机需要保证相机的像素格式可以调整为`Bayer RG 8`且同样遵循`USB 3.0 Vision`协议。
- 使用前需要在MVS上位机软件中调试好相机的曝光时间、数字补偿、模拟补偿、亮度等参数，将相机的像素格式调整为`Bayer RG 8`（其他型号相机同理），欲使用其他像素格式需要自行修改项目，调整完成后设置为启动默认参数。
### 编译`Python`可用的链接共享库
进入目录`HIK_TEF_Driver_module`，键入`sudo python3 setup.py build_ext --inplace`进行编译，若弹出有关`Numpy API`的警告可忽略，终端输出绿色字样`Setup has been completed!`即表示编译成功，此时目录`HIK_TEF_Driver_module`下会生成`__HIK_TEF_Driver__.cpp`文件、`.so`后缀的链接共享库和`build`目录。  
在项目目录下的python脚本中，可以以下语句引入相机驱动库。  
```
from HIK_TEF_Driver_module import HIK_TEF_Driver
```
欲在其他路径文件中引入相机驱动库，请将`HIK_TEF_Driver_module`目录复制到相应路径下或加入环境路径。  
欲在`sudo`环境下运行驱动库还需要将`HIK_TEF_Driver_module/lib`目录与`HIK_TEF_Driver_module/include`目录加入`root`环境路径。  

### 运行示例测试程序
## 文件结构
-`HIK_TEF_Driver_module/include`：
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

