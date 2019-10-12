**注意** 此分支只能用于测试网关功能，不可用于产品开发

使用方法
========
1、下载nRF5 SDK 15.2.0 [here](https://www.nordicsemi.com/Software-and-Tools/Software/nRF5-SDK)

建议将SDK下载到硬盘根目录下，过长的目录会导致keil编译失败

2、进入 SDK 15.2.0\examples\ble_peripheral\ 目录.
```
 $ git clone -b test/mibeacon_v5 --recursive https://github.com/MiEcosystem/mijia_ble.git
```

3、打开 工程 pca10040\s132\arm5_no_packs\ble_app_template_pca10040_s132.uvprojx, 编译下载到 nRF52832 开发板 PCA 10040




