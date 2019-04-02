此分支为TI SimpleLink平台 CC2640R2的米家蓝牙标准接入SDK，在SDK基础上提供库以及示例demo。

使用:

1、下载CC2640R2 SDK 2.40.00.32 [here](http://www.ti.com/tool/download/SIMPLELINK-CC2640R2-SDK)
   建议将SDK安装在默认目录C:/ti下面

2、下载米家标准认证示例demo

3、cd in C:\ti\simplelink_cc2640r2_sdk_2_40_00_32 directory.
   将此软件包解压，会增加examples\rtos\CC2640R2_LAUNCHXL\ble5stack\mible_authen和source\ti\ble5stack\mible

4、将从米家开放平台上申请得到的mible_app_authen-v1.1.2-0110.a拷贝到source\ti\ble5stack\mible\libs文件夹下
   米家蓝牙标准认证库闭源，请联系 https://iot.mi.com/ 平台申请，详见https://iot.mi.com/guide.html#id=57

5、CC2640R2工程路径：examples\rtos\CC2640R2_LAUNCHXL\ble5stack\mible_authen\tirtos, 提供CCS和IAR两种参考

6、关于米家蓝牙标准认证库的使用说明如下链接：https://miecosystem.github.io/mijia_ble/

注意: 建议使用下面版本及以上的IAR或CCS
      IAR Embedded Workbench: EWARM-8.20.2
      TI Code Composer Studio: CCS-8.3.0.00009