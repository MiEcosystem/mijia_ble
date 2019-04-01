此分支为Cypress CYW20706A2米家蓝牙标准接入SDK，在原厂SDK基础上提供库以及示例demo。

### 使用

1、下载WICED-Studio 6.x Installer。 如：
https://community.cypress.com/servlet/JiveServlet/downloadBody/15206-102-3-17354/WICED-Studio-6.2.1.2-IDE-Installer.zip
免费注册账户，请按说明安装、使用此SDK。

2、下载米家标准认证示例demo

2.1  
$ git clone -b cypress --recursive https://github.com/MiEcosystem/mijia_ble.git

2.2 
把mijia_std_authen_wiced_20706_v1.0.4.a放到WICED-Studio-6.x \20706-A2_Bluetooth\apps\demo\

2.3 
把#define version      (*(volatile unsigned int *)version_adr) 从如下文件中删除

WICED-Studio-6.x\20706-A2_Bluetooth\Wiced-BT\bsp\inc\20703mapa0.h

2.4
添加如下make target：demo.mijia_ble.demos.cypress.mible_std_authen-CYW920706WCDEVAL download

3、
将从米家开放平台上申请得到的mijia_std_authen_wiced_20706_vxxxx.a拷贝到 WICED-Studio-6.x\20706-A2_Bluetooth\Wiced-BT\tier2\brcm\libraries\lib\20703文件夹下

米家蓝牙标准认证库闭源，请联系 https://iot.mi.com/ 平台申请，详见https://iot.mi.com/guide.html#id=57

4、
关于米家蓝牙标准认证库的使用说明如下链接：https://miecosystem.github.io/mijia_ble/


### 补充
Cypress WICED开发者论坛：
https://community.cypress.com/content

参考设计图及使用说明：

WICED-Studio-6.x\Doc\CYW920706WCDEVAL-Hardware-User-Guide.pdf

WICED-Studio-6.x\Doc\WICED-CYW920706WCDEVAL-Kit-Guide.pdf

USB-UART串口驱动：

WICED-Studio-6.x\Drivers\Windows

参考示例程序：

WICED-Studio-6.x\20706-A2_Bluetooth\apps

API使用说明：

WICED-Studio-6.x\Doc\20706-A2_Bluetooth\ API.html

工厂测试文档及工具：

WICED-Studio-6.x\Doc\Manufacturing-Bluetooth-Test-Tool.pdf

WICED-Studio-6.x\wiced_tools\wmbt
