此分支为芯海科技CST92F30平台的米家蓝牙标准接入SDK，在原厂SDK基础上提供库以及示例demo。

### 使用
1、请联系芯海科技 support@chipsea.com 获取CST92F30SDK，目前适配版本为CST92F30SDK 1.1。

建议将CST92F30SDK下载到硬盘根目录下，过长的目录会导致keil编译失败

2、下载米家标准认证示例demo

cd in CST92F30SDK\projects\ble_peripheral\ directory.

 $ git clone -b Chipsea --recursive https://github.com/MiEcosystem/mijia_ble.git

3、将从米家开放平台上申请得到的mijia_std_authen_keil.lib拷贝到mijia_ble\src\mijia_libs\mible_app_authen文件夹下

米家蓝牙标准认证库闭源，请联系 https://iot.mi.com/ 平台申请，详见https://iot.mi.com/guide.html#id=57

4、mijia_ble工程路径：CST92F30SDK\projects\ble_peripheral\mijia_ble\keil

可以在KEIL5中运行

5、关于米家蓝牙标准认证库的使用说明如下链接：https://miecosystem.github.io/mijia_ble/ 
