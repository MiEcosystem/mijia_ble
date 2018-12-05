此分支为Nordic52平台的米家蓝牙标准接入SDK，在原厂SDK 15.2基础上提供库以及示例demo。

### 使用
1、下载nRF5 SDK 15.2.0 [here](https://www.nordicsemi.com/eng/nordic/Products/nRF52-DK/nRF5-SDK-zip/59014)

建议将SDK下载到硬盘根目录下，过长的目录会导致keil编译失败

2、下载米家标准认证示例demo

cd in SDK 15.2.0\examples\ble_peripheral\ directory.

 $ git clone -b Nordic15.2 --recursive https://github.com/MiEcosystem/mijia_ble.git

3、将从米家开放平台上申请得到的mijia_std_authen_keil.lib拷贝到mijia_ble\mijia_ble_libs\mible_app_authen文件夹下

米家蓝牙标准认证库闭源，请联系 https://iot.mi.com/ 平台申请，详见https://iot.mi.com/guide.html#id=57

4、Nordic52工程路径：mijia_ble\pca10040\s132\arm5_no_packs

可以在KEIL5中运行

5、关于米家蓝牙标准认证库的使用说明如下链接：https://miecosystem.github.io/mijia_ble/ 
