此分支为米家蓝牙接入 Nordic51/52平台的KEIL平台SDK。

Nordic51工程路径：mijia_ble/demos/Nordic/mible_std_authen/pca10028/s130/arm5_no_packs/

Nordic52工程路径：mijia_ble/demos/Nordic/mible_std_authen/pca10040/s132/arm5_no_packs/

SDK中包含Nordic平台对米家蓝牙统一接口的适配、米家蓝牙标准认证库以及demo示例。

关于米家蓝牙标准认证库的使用说明如下链接：https://miecosystem.github.io/mijia_ble/ 

关于米家蓝牙统一接口的说明如下链接： https://miecosystem.github.io/mijia_ble_api/ ，此部分应用层不需关心。

米家蓝牙标准认证库闭源，请联系 https://iot.mi.com/ 平台申请，详见https://iot.mi.com/guide.html#id=57

#### How to use
Nordic Platform:
1. download nRF5 SDK 12.3.0 [here](http://www.nordicsemi.com/eng/nordic/Products/nRF52832/nRF5-SDK-v12-zip/54281)
2. cd in SDK 12.3.0\examples\ble_peripheral\ directory.
3. $ git clone --recursive https://github.com/MiEcosystem/mijia_ble.git
4. $ cd mijia_ble
5. $ git checkout Nordic
