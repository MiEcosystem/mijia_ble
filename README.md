# Hardware Platform
- nRF52832-DK PCA10040
- nRF52840-DK PCA10056

*<!> nRF51 demo project is in the branch [**nordic_legacy**](https://github.com/MiEcosystem/mijia_ble_secure/tree/nordic_legacy)*

# Requirement
- arm MDK 5.25
- SEGGER Jlink
- Git

# How to use

1. You need to download nRF5 SDK 15.2.0 from [here](https://www.nordicsemi.com/eng/nordic/Products/nRF52-DK/nRF5-SDK-zip/59014)
2. $ cd SDK_15.2.0\examples\ble_peripheral\ directory.
3. $ git clone --recursive https://github.com/MiEcosystem/mijia_ble_secure.git -b nordic
3. change PRODUCT_ID to your product ID (i.e. pid), that you got when registered in [Mi IoT](https://iot.mi.com/index.html). The default PID is stand for Xiaomi BLE secure devboard.

# Diagnose
### Unix-like system
Make sure you have installed JLink and add it to your path. <br>
$ JLinkExe -device NRF52832_XXAA -if swd -speed 1000 -RTTTelnetPort 2000 <br>
or<br>
$ JLinkExe -device NRF52840_XXAA -if swd -speed 1000 -RTTTelnetPort 2000 <br>
Then open a new terminal tab and execute:<br>
$ telnet 127.0.0.1 2000<br>
You will find the log information in this telnet session.

### windows 
Use the J-Link RTT Viewer to get those log information.