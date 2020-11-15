# AI_Composer_on_FPGA
------



本作品旨在设计能够在硬件端实现AI编曲的SoC，对复调音乐编曲并进行实时推理和播放。其中以Arm Cortex-m0为控制器内核，对多种板载外设进行调度，同时搭载基于GRU的神经网络硬件加速器，用来推理和谐的音符序列，最后由音频DAC实现音频输出。

本项目开源的内容是神经网络加速器模块以及ahb接口模块的verilog设计，以及软件的c程序。而arm软核和soc部分未上传至github。

但作品的全部代码工程已上传至FPGA创新设计竞赛（包括加速器模块、SoC和软件程序等所有工程）
