﻿================================================================================
                                样例使用说明
================================================================================
版本历史
日期           作者        IAR     MDK     GCC     描述
2018-11-14     CDT         7.70    5.16    8.3.1   first version
================================================================================
平台说明
================================================================================
GCC工程，由Eclipse IDE外挂GNU-ARM Toolchain，再结合pyOCD GDB Server实现工程的编译、
链接和调试。在用Eclipse导入工程后，请将xxxx_PyOCDDebug中pyocd-gdbserver和SVD文件
设置为正确的路径；请将xxxx_PyOCDDownload中pyocd设置为正确的路径。注意，这些路径不
能包含非英文字符。


功能描述
================================================================================
本样例为芯片进入power down模式1，通过WKUP00（或NMI）唤醒


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
无

================================================================================
使用步骤
================================================================================
1）打开工程并重新编译；
2）启动IDE的下载和调试功能，全速运行；
3）可观察到红灯闪烁两下，然后芯片进入掉电模；
4）连接PA0和PD3，按下SW2，唤醒芯片；（用NMI事件，连接NMI管脚（PB11）和GND,可唤醒芯片）
5）可观察到红灯闪烁两下（表明芯片已被唤醒，继续执行用户代码）。


================================================================================
注意
================================================================================
通过修改PWC_PD_WAKEUP_EVENT定义选择NMI事件唤醒，还是wakeup00事件唤醒
define PWC_PD_WAKEUP_EVENT             (PWC_PDWKEN0_WKUP00)
或
define PWC_PD_WAKEUP_EVENT             (PWC_PDWKEN2_NMI)
================================================================================
