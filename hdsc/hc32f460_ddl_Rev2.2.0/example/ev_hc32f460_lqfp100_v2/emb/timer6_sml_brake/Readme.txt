﻿================================================================================
                                样例使用说明
================================================================================
版本历史
日期           作者        IAR     MDK     GCC     描述
2021-04-16     CDT        7.70    5.16    8.3.1   first version
================================================================================
平台说明
================================================================================
GCC工程，由Eclipse IDE外挂GNU-ARM Toolchain，再结合pyOCD GDB Server实现工程的编译、
链接和调试。在用Eclipse导入工程后，请将xxxx_PyOCDDebug中pyocd-gdbserver和SVD文件
设置为正确的路径；请将xxxx_PyOCDDownload中pyocd设置为正确的路径。注意，这些路径不
能包含非英文字符。


功能描述
================================================================================
本样例主要展示MCU的EMB模块的PWM同相短路刹车功能（同低检测）。
说明：
EMB1用于Timer6
（EMB2-4用于Timer4）
 EMB1中断内设置软件刹车，清除同高同低刹车标志
在while（1）内做刹车处理，处理完后使能PWM输出
================================================================================
测试环境
================================================================================
测试用板:
---------------------
EV-F460-LQ100-V2.0

辅助工具:
---------------------
示波器

辅助软件:
---------------------
无

================================================================================
使用步骤
================================================================================
1） 示波器查看PE9和PE8
2） 编译，下载并全速运行。
3） 当PWMA和PWMB同时输出低电平时，可以看到PWM输出变为高电平，LED_RED亮起
4） 延时之后PWM恢复输出
   （由于timer6的输出配置为同高同低，所以一旦PWM输出恢复会再次产生同低刹车保护并进入中断）。
================================================================================
注意
================================================================================


================================================================================
