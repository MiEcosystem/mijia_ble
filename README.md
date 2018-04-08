此分支为Diaglog14585平台的米家蓝牙标准接入SDK，米家在原厂SDK基础上，提供库以及示例demo。

### 使用

1、先下载Dialog官方SDK，建议下载到硬盘根目录下，过长的目录会导致keil编译失败。

版本： 6.0.6.427

从Dialog support 官网 https://support.dialog-semiconductor.com/  可供下载官方SDK，
https://support.dialog-semiconductor.com/
此网站下载默认需要账号，可以根据提示申请，申请需要注意使用公司邮箱和正确的公司地址，Dialog会根据实际去审核信息。也可以找Dialog芯片厂商或者芯片代理商咨询提供

2、下载米家标准认证示例demo

cd in SDK 6.0.6.427\projects\target_apps\ble_examples目录下

$  git clone -b Dialog --recursive https://github.com/MiEcosystem/mijia_ble.git

$ cd mijia_ble

3、将从米家开放平台上申请得到的mijia_std_authen_keil_DA14585.lib拷贝到mijia_ble\libs\mible_app_authen文件夹下

米家蓝牙标准认证库闭源，请联系 https://iot.mi.com/ 平台申请，详见https://iot.mi.com/guide.html#id=57

4、工程路径：6.0.6.427\projects\target_apps\ble_examples\mijia_app_authen\Keil_5 运行工程

下载程序方式说明和其他开发资料可在https://support.dialog-semiconductor.com/中下载

5、 关于米家蓝牙标准认证库的使用说明如下链接：https://miecosystem.github.io/mijia_ble/

工程中mijia_ble\demos\Dialog\mible_std_authen\Dialog14585\src\user为调用示例

### 说明

#### FLASH
dialog DA14585 没有内部的FLASH可供使用，代码可放置在片外的存储中或者是片内的OTP(64KByte)。片外存储可以选择EEprom、FLASH，绝大部分客户使用FLASH,官方EEprom默认支持使用IIC通信，FLASH默认支持使用SPI通信，在通信速率和价格上，使用FLASH价格会比EEprom便宜、和快速。由于米家接口的支持需要使用到存储信息,故建议使用外挂的FLASH作为存储。如果使用dialog DA14586待内置存储的需要对应修改。使用dialog DA14586内置Flash会比dialog DA14585 + 片外FLASH会贵一些。这里只提供DA14585 的米家服务的示例，如果需要使用到586芯片可自行移植flash存储部分或者找芯片厂商或者代理商支持。

#### 内存问题
米家服务使用了动态申请内存,  在下载官网skd后 注意调整startup_ARMCM0.S  文件中 Heap_Size       EQU     0x00000400
