﻿================================================================================
                                样例使用说明
================================================================================
版本历史 
日期            作者         IAR     MDK     GCC     描述
2021-04-16      CDT         7.70    5.16    8.3.1   first version
================================================================================
平台说明
================================================================================
GCC工程，由Eclipse IDE外挂GNU-ARM Toolchain，再结合pyOCD GDB Server实现工程的编译、
链接和调试。在用Eclipse导入工程后，请将xxxx_PyOCDDebug中pyocd-gdbserver和SVD文件
设置为正确的路径；请将xxxx_PyOCDDownload中pyocd设置为正确的路径。注意，这些路径不
能包含非英文字符。


功能描述
================================================================================
本样例为EFM模块中一次性可编程字节（otp）样例。
本样例通过const数组直接对otp区域及其锁存区域进行操作。


================================================================================
测试环境
================================================================================
测试用板:
---------------------
EV-F460-LQ100-V2.0

辅助工具:
---------------------
无

辅助软件:
---------------------
无

================================================================================
使用步骤
================================================================================
1）打开工程并重新编译；
2）启动IDE的下载和调试功能，全速运行；
3）打开memory窗口，可看到otp区域与main.c中预期值一致，锁存区域全为0.


================================================================================
注意
================================================================================
当使用keil工程，需在option->Debug->Settings->Flash Download里添加HC32F460 otp flash

================================================================================
