#### Hardware Platform
* nRF51-DK PCA10028 
* nRF52-DK PCA10040

#### Profile

|        | bytes   | nRF52 (us) | nRF51 (us) |
| ------ | ------: | ------:    |------:     |
| aesecb | 16      | 14         | 51         |
| aesccm | 16      | 93         | 360        |
| -      | 32      | 128        | 500        |
| hkdf   | 32      | 490        | 3400       |
| -      | 64      | 780        | 6000       |

#### How to use

1. download nRF5 SDK 12.3.0 [here](http://www.nordicsemi.com/eng/nordic/Products/nRF52832/nRF5-SDK-v12-zip/54281)
2. clone this repo in SDK 12.3.0\examples\ble_peripheral\ directory.