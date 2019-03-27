此分支为telink平台的米家蓝牙标准接入SDK，在原厂SDK 3.2基础上提供库以及示例demo。

###使用
1、联系telink技术支持下载SDK 3.2 [here](http://www.telink-semi.cn/viewtopic.php?f=7&t=644)

建议将SDK下载到硬盘根目录下，过长的目录可能会导致编译失败

2、下载米家标准认证示例demo

cd in ./vendor/common/ directory.

$ git clone -b telink --recursive https://github.com/MiEcosystem/mijia_ble.git

3、将从米家开放平台上申请得到的libmijia_std_authen_eclipse_telink_1_1_4.a拷贝到/proj_lib文件夹下

米家蓝牙标准认证库闭源，请联系 https://iot.mi.com/ 平台申请，详见https://iot.mi.com/guide.html#id=57

4、具体demo工程的使用方法请联系telink技术支持

5、关于米家蓝牙标准认证库的使用说明如下链接：https://miecosystem.github.io/mijia_ble/
