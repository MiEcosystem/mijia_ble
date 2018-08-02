此分支为Silicon Labs BG13平台的米家蓝牙标准接入SDK，米家在原厂SDK基础上，提供库以及示例demo。

### 使用

1. 下载安装Simplicity Studio 集成开发环境, 并在软件中下载Bluetooth SDK 2.9.2.0 ;

2. 下载米家标准认证示例demo,  进入SimplicityStudio/v4_workspace目录

   git clone -b Silabs --recursive <https://github.com/MiEcosystem/mijia_ble.git>

3. 将从米家开放平台上申请得到的mijia_std_authen_Simplicity_BG13_1_1_2.a拷贝到mijia_ble\libs\mible_app_authen文件夹下,  米家蓝牙标准认证库闭源，请联系 <https://iot.mi.com/> 平台申请，详见<https://iot.mi.com/guide.html#id=57>

4. 将mijia_ble工程import到 Simplicity studio 中. 此demo工程可以在BG13官方开发板或米家BG13模组上运行,在米家APP中添加蓝牙设备，可以搜索到小米蓝牙开发板，完成绑定和登录。

5. 关于米家蓝牙标准认证库的使用说明如下链接：<https://miecosystem.github.io/mijia_ble/>

   