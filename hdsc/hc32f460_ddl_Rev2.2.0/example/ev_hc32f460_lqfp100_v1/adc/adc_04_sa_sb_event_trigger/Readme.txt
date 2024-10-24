﻿================================================================================
                                样例使用说明
================================================================================
版本历史
日期           作者      IAR     MDK     GCC     描述
2018-11-30     CDT      7.70    5.16    8.3.1   first version
================================================================================
平台说明
================================================================================
GCC工程，由Eclipse IDE外挂GNU-ARM Toolchain，再结合pyOCD GDB Server实现工程的编译、
链接和调试。在用Eclipse导入工程后，请将xxxx_PyOCDDebug中pyocd-gdbserver和SVD文件
设置为正确的路径；请将xxxx_PyOCDDownload中pyocd设置为正确的路径。注意，这些路径不
能包含非英文字符。


功能描述
================================================================================
1）实现了 ADC1 和 ADC2 双序列（序列 A 和序列 B）单次扫描模式的用法；
2）ADC1 序列 B 由 PB7 下降沿触发，序列 B 扫描结束触发序列 A 扫描；ADC2 序列 A 由
   PB6 下降沿触发，序列 A 扫描结束触发序列 B 扫描。

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
串口调试助手（115200bps，8N1）

================================================================================
使用步骤
================================================================================
1）打开工程并重新编译；
2）Micro USB 插入 USBDAP 将开发板与电脑相连；
3）启动 IDE 的调试功能，断点查看程序运行情况；或全速运行用串口调试助手查看 ADC1 序列
   A、B 和 ADC2 序列 A、B 的触发情况；
4）在 PB7 上制造下降沿可触发 ADC1 序列 B 扫描；
5）在 PB6 上制造下降沿可触发 ADC2 序列 A 扫描。

================================================================================
注意
================================================================================
1）ADC 的参考电压来自引脚 VREFH。

================================================================================
