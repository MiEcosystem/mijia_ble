此分支为NXPQN908X平台的米家蓝牙接入demo。

NXPQN908X工程路径：mijia_ble\demos\NXP\mible_std_authen\NXPQN980X\bm\mdk

SDK中包含NXPQN908X平台对米家蓝牙统一接口的适配、米家蓝牙标准认证库以及demo示例。

关于米家蓝牙标准认证库的使用说明如下链接：https://miecosystem.github.io/mijia_ble/

关于米家蓝牙统一接口的说明如下链接： https://miecosystem.github.io/mijia_ble_api/ ，此部分应用层不需关心。

米家蓝牙标准认证库闭源，请联系 https://iot.mi.com/ 平台申请，详见https://iot.mi.com/guide.html#id=57

How to use
Nordic Platform:

下载NXPQN908X在线生成的sdk： https://mcuxpresso.nxp.com/en/builder (芯片型号 QN980X; windows；keil平台)

建议将SDK下载到硬盘根目录下，过长的目录会导致keil编译失败

cd in SDK SDK_2.2_QN908XCDK_RC4 \boards\qn908xcdk\wireless_examples\bluetooth目录下

$ git clone https://github.com/MiEcosystem/mijia_ble.git

$ cd mijia_ble

$ git checkout NXP

$ git submodule init 

$ git submodule update

将从平台上申请得到的mijia_std_authen_keil_NXPQN980X.lib拷贝到mijia_ble\libs\mible_app_authen文件夹下

修改SDK文件fw_symbols_mdk.h，添加一行：0x030254c5 A nvds_del

此demo工程可以在NXPQN908X开发板上运行，上电后按BUTTON1开启广播，在米家APP中添加蓝牙设备，可以搜索到小米蓝牙开发板，可以完成绑定和登录。
