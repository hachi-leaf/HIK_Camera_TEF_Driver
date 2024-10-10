from setuptools import setup
from setuptools import Extension
from Cython.Build import cythonize
import numpy as np

# Usage: $ sudo python3 setup.py build_ext --inplace
# sudo python setup.py install

# 为编译器获取numpy路径
try:
    numpy_include = np.get_include()
except AttributeError:
    numpy_include = np.get_numpy_include()

# 源文件路径
source_files = ["__HIK_TEF_Driver__.pyx", "HIK_TEF_Driver.cpp"]

# 链接库路径
library_dirs = ["./lib"]

# 头文件路径
include_dirs = [numpy_include, "/usr/include/opencv4", './include']

# 链接库名
libraries = [
    "opencv_stitching",
    "opencv_alphamat", 
    "opencv_aruco", 
    "opencv_barcode", 
    "opencv_bgsegm", 
    "opencv_bioinspired", 
    "opencv_ccalib", 
    "opencv_dnn_objdetect", 
    "opencv_dnn_superres", 
    "opencv_dpm", 
    "opencv_face", 
    "opencv_freetype", 
    "opencv_fuzzy", 
    "opencv_hdf", 
    "opencv_hfs", 
    "opencv_img_hash", 
    "opencv_intensity_transform", 
    "opencv_line_descriptor", 
    "opencv_mcc", 
    "opencv_quality", 
    "opencv_rapid", 
    "opencv_reg", 
    "opencv_rgbd", 
    "opencv_saliency", 
    "opencv_shape", 
    "opencv_stereo", 
    "opencv_structured_light", 
    "opencv_phase_unwrapping", 
    "opencv_superres", 
    "opencv_optflow", 
    "opencv_surface_matching", 
    "opencv_tracking", 
    "opencv_highgui", 
    "opencv_datasets", 
    "opencv_text", 
    "opencv_plot", 
    "opencv_ml", 
    "opencv_videostab", 
    "opencv_videoio", 
    "opencv_viz", 
    "opencv_wechat_qrcode", 
    "opencv_ximgproc", 
    "opencv_video", 
    "opencv_xobjdetect", 
    "opencv_objdetect", 
    "opencv_calib3d", 
    "opencv_imgcodecs", 
    "opencv_features2d", 
    "opencv_dnn", 
    "opencv_flann", 
    "opencv_xphoto", 
    "opencv_photo", 
    "opencv_imgproc", 
    "opencv_core",
    "FormatConversion",
    "MediaProcess",
    "MvCameraControl",
    "MVRender",
    "MvUsb3vTL"]

# 组成ext对象
ext_modules = [Extension(
    "HIK_TEF_Driver", 
    source_files, 
    language="c++",
    library_dirs=library_dirs,
    include_dirs=include_dirs,
    libraries = libraries,
    define_macros=[('NPY_NO_DEPRECATED_API', 'NPY_1_7_API_VERSION')]
    )]

# import name: HIK_TEF_Driver
setup(
    name="HIK_TEF_Driver", 
    ext_modules=cythonize(ext_modules))

print("\033[32mSetup has been completed!\033[0m")
