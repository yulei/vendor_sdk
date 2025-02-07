﻿================================================================================
                                样例使用说明
================================================================================
版本历史
日期          作者         IAR     MDK     GCC     描述
2018-11-27    CDT          7.70    5.16    8.3.1   first version
================================================================================
平台说明
================================================================================
GCC工程，由Eclipse IDE外挂GNU-ARM Toolchain，再结合pyOCD GDB Server实现工程的编译、
链接和调试。在用Eclipse导入工程后，请将xxxx_PyOCDDebug中pyocd-gdbserver和SVD文件
设置为正确的路径；请将xxxx_PyOCDDownload中pyocd设置为正确的路径。注意，这些路径不
能包含非英文字符。


功能描述
================================================================================
本样例主要展示USART外设配置为UART模式时通过超时功能监测接收。
说明：
无

================================================================================
测试环境
================================================================================
测试用板:
---------------------
EV-HC32F460-LQFP100-050-V1.3


辅助工具:
---------------------
无

辅助软件:
---------------------
串口助手软件

================================================================================
使用步骤
================================================================================
1）通过USB数据线，连接PC和板子J1。
2）打开串口助手软件，配置端口如下参数。
     波特率：115200
     数据位：8
     校验位：None
     停止位：1
3）打开工程并重新编译。
4）下载代码测试板，并且全速运行。
5）在串口助手窗口连续输入字母或数字，窗口回显输入内容，LED_RED熄灭。
6）停止输入，约3秒后，LED_RED点亮，表示设定的超时范围内未收到数据。
7）重复步骤5）~ 6）。

================================================================================
注意
================================================================================
如使用时更改USART通道，需同时修改UART外设通道，GPIO配置、Timer0单元和通道、外设
Clock使能和配置NVIC功能的相应事件号。

================================================================================
