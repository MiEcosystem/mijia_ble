此分支为NXPQN908X、NXPQN902X平台的米家蓝牙标准接入SDK，米家在原厂SDK基础上，提供库以及示例demo。

### 使用

#### NXPQN908X
1、先下载NXPQN908X在线生成的sdk：https://mcuxpresso.nxp.com/en/builder (芯片型号 QN980X; windows；keil平台)

建议将SDK下载到硬盘根目录下，过长的目录会导致keil编译失败

2、下载米家标准认证示例demo

cd in SDK SDK_2.2_QN908XCDK_RC4 \boards\qn908xcdk\wireless_examples\bluetooth目录下

$ git clone -b NXP --recursive https://github.com/MiEcosystem/mijia_ble.git

3、将从米家开放平台上申请得到的mijia_std_authen_keil_NXPQN908X.lib拷贝到mijia_ble\libs\mible_app_authen文件夹下

米家蓝牙标准认证库闭源，请联系 https://iot.mi.com/ 平台申请，详见https://iot.mi.com/guide.html#id=57

4、修改SDK文件fw_symbols_mdk.h，添加一行：0x030254c5 A nvds_del

5、Demo工程位置在mijia_ble\demos\NXP\mible_std_authen\NXPQN980X\bm\mdk，在keil5中运行。此demo工程可以在NXPQN908X开发板上运行，上电后按BUTTON1开启广播，在米家APP中添加蓝牙设备，可以搜索到小米蓝牙开发板，可以完成绑定和登录。

6、关于米家蓝牙标准认证库的使用说明如下链接：https://miecosystem.github.io/mijia_ble/

#### NXPQN902X
1、下载并安装QN902x的SDK，链接https://www.nxp.com/downloads/en/software-development-kits/QN902x_SDK_140.zip；

2、下载米家标准认证示例demo

cd in SDK QN902x_SDK_1.4.0\Projects\BLE目录下

$ git clone -b NXP --recursive https://github.com/MiEcosystem/mijia_ble.git

3、将工程mijia_ble\demos\NXP_QN902X目录下的src.zip剪切到工程SDK的QN902x_SDK_1.4.0\Projects\BLE目录下，并将文件合并覆盖原有的src文件夹。

4、将从米家开放平台上申请得到的mijia_std_authen_keil_NXPQN902X.lib 拷贝到mijia_ble\libs\mible_app_authen文件夹下

米家蓝牙标准认证库闭源，请联系 https://iot.mi.com/ 平台申请，详见https://iot.mi.com/guide.html#id=57

5、Demo工程位置在mijia_ble\demos\NXP_QN902X\mible_std_authen\keil目录下，在keil5中运行。 

6、关于米家蓝牙标准认证库的使用说明如下链接：https://miecosystem.github.io/mijia_ble/
