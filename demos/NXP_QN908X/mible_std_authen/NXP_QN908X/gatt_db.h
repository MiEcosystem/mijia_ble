PRIMARY_SERVICE(service_gatt, gBleSig_GenericAttributeProfile_d)
        CHARACTERISTIC(char_service_changed, gBleSig_GattServiceChanged_d, (gGattCharPropRead_c | gGattCharPropNotify_c) )
            VALUE(value_service_changed, gBleSig_GattServiceChanged_d, (gPermissionNone_c), 4, 0x00, 0x00, 0x00, 0x00)
            CCCD(cccd_service_changed)

PRIMARY_SERVICE(service_gap, gBleSig_GenericAccessProfile_d)
    CHARACTERISTIC(char_device_name, gBleSig_GapDeviceName_d, (gGattCharPropRead_c) )
            VALUE(value_device_name, gBleSig_GapDeviceName_d, (gPermissionFlagReadable_c), 12, "NXP_BLE_QPPS")
    CHARACTERISTIC(char_appearance, gBleSig_GapAppearance_d, (gGattCharPropRead_c) )
            VALUE(value_appearance, gBleSig_GapAppearance_d, (gPermissionFlagReadable_c), 2, UuidArray(gGenericHeartrateSensor_c))
    CHARACTERISTIC(char_ppcp, gBleSig_GapPpcp_d, (gGattCharPropRead_c) )
            VALUE(value_ppcp, gBleSig_GapPpcp_d, (gPermissionFlagReadable_c), 8, 0x0A, 0x00, 0x10, 0x00, 0x64, 0x00, 0xE2, 0x04)

PRIMARY_SERVICE_UUID128(service_qpps, uuid_service_qpps)
    CHARACTERISTIC_UUID128(char_qpps_rx, uuid_qpps_characteristics_rx, (gGattCharPropWriteWithoutRsp_c) )
        VALUE_UUID128_VARLEN(value_qpps_rx, uuid_qpps_characteristics_rx, (gPermissionFlagWritable_c), 512, 1, 0x00)
    CHARACTERISTIC_UUID128(char_qpps_tx, uuid_qpps_characteristics_tx, (gGattCharPropNotify_c))
        VALUE_UUID128_VARLEN(value_qpps_tx, uuid_qpps_characteristics_tx, (gPermissionNone_c), 512, 2, 0x00, 0xB4)
        CCCD(cccd_qpps_tx)
    //chx
    CHARACTERISTIC_UUID128(char_qpps_period_tx, uuid_qpps_characteristics_period_tx, (gGattCharPropIndicate_c))
        VALUE_UUID128_VARLEN(value_qpps_period_tx, uuid_qpps_characteristics_period_tx, (gPermissionNone_c), 512, 4, 0x00, 0x01, 0x02, 0x03)
        CCCD(cccd_qpps_period_tx)

PRIMARY_SERVICE(service_battery, gBleSig_BatteryService_d)
    CHARACTERISTIC(char_battery_level, gBleSig_BatteryLevel_d, (gGattCharPropNotify_c | gGattCharPropRead_c))
        VALUE(value_battery_level, gBleSig_BatteryLevel_d, (gPermissionFlagReadable_c), 1, 0x5A)
        DESCRIPTOR(desc_bat_level, gBleSig_CharPresFormatDescriptor_d, (gPermissionFlagReadable_c), 7, 0x04, 0x00, 0xAD, 0x27, 0x01, 0x01, 0x00)
        CCCD(cccd_battery_level)

PRIMARY_SERVICE(service_device_info, gBleSig_DeviceInformationService_d)
    CHARACTERISTIC(char_manuf_name, gBleSig_ManufacturerNameString_d, (gGattCharPropRead_c) )
        VALUE(value_manuf_name, gBleSig_ManufacturerNameString_d, (gPermissionFlagReadable_c), sizeof(MANUFACTURER_NAME), MANUFACTURER_NAME)
    CHARACTERISTIC(char_model_no, gBleSig_ModelNumberString_d, (gGattCharPropRead_c) )
        VALUE(value_model_no, gBleSig_ModelNumberString_d, (gPermissionFlagReadable_c), 9, "QPPS Demo")
    CHARACTERISTIC(char_serial_no, gBleSig_SerialNumberString_d, (gGattCharPropRead_c) )
        VALUE(value_serial_no, gBleSig_SerialNumberString_d, (gPermissionFlagReadable_c), 7, "BLESN01")
    CHARACTERISTIC(char_hw_rev, gBleSig_HardwareRevisionString_d, (gGattCharPropRead_c) )
        VALUE(value_hw_rev, gBleSig_HardwareRevisionString_d, (gPermissionFlagReadable_c), sizeof(BOARD_NAME), BOARD_NAME)
    CHARACTERISTIC(char_fw_rev, gBleSig_FirmwareRevisionString_d, (gGattCharPropRead_c) )
        VALUE(value_fw_rev, gBleSig_FirmwareRevisionString_d, (gPermissionFlagReadable_c), 5, "1.1.1")
    CHARACTERISTIC(char_sw_rev, gBleSig_SoftwareRevisionString_d, (gGattCharPropRead_c) )
        VALUE(value_sw_rev, gBleSig_SoftwareRevisionString_d, (gPermissionFlagReadable_c), 5, "1.1.4")
    CHARACTERISTIC(char_system_id, gBleSig_SystemId_d, (gGattCharPropRead_c) )
        VALUE(value_system_id, gBleSig_SystemId_d, (gPermissionFlagReadable_c), 8, 0x00, 0x00, 0x00, 0xFE, 0xFF, 0x9F, 0x04, 0x00)
    CHARACTERISTIC(char_rcdl, gBleSig_IeeeRcdl_d, (gGattCharPropRead_c) )
        VALUE(value_rcdl, gBleSig_IeeeRcdl_d, (gPermissionFlagReadable_c), 4, 0x00, 0x00, 0x00, 0x00)

        
#define MIBLE_SRV_UUID                         	0xFE95//{0x95,0xFE}  
#define MIBLE_STD_CHAR_NUM					   	7
#define MIBLE_CHAR_UUID_TOKEN                  	0x0001
#define MIBLE_CHAR_UUID_PRODUCTID              	0x0002
#define MIBLE_CHAR_UUID_VERSION                	0x0004
#define MIBLE_CHAR_UUID_WIFICFG                	0x0005
#define MIBLE_CHAR_UUID_AUTHENTICATION         	0x0010
#define MIBLE_CHAR_UUID_DID                    	0x0013
#define MIBLE_CHAR_UUID_BEACONKEY              	0x0014
#define MIBLE_CHAR_UUID_DEVICE                 	0x0015
#define MIBLE_CHAR_UUID_SECURE                 	0x0016

#define MIBLE_CHAR_LEN_TOKEN                   	12
#define MIBLE_CHAR_LEN_PRODUCTID               	2
#define MIBLE_CHAR_LEN_VERSION                 	10
#define MIBLE_CHAR_LEN_WIFICFG                 	20
#define MIBLE_CHAR_LEN_AUTHENTICATION          	4
#define MIBLE_CHAR_LEN_DID                     	20
#define MIBLE_CHAR_LEN_BEACONKEY               	12
#define MIBLE_CHAR_LEN_DEVICE                  	20
#define MIBLE_CHAR_LEN_SECURE                  	20

PRIMARY_SERVICE(mi_service, MIBLE_SRV_UUID)
	CHARACTERISTIC(mi_char_token, MIBLE_CHAR_UUID_TOKEN, (gGattCharPropNotify_c | gGattCharPropWrite_c))
		VALUE(value_char_token, MIBLE_CHAR_UUID_TOKEN, (gPermissionFlagWritable_c), MIBLE_CHAR_LEN_TOKEN, 0x00)
		CCCD(cccd_char_token)
	CHARACTERISTIC(mi_char_pid, MIBLE_CHAR_UUID_PRODUCTID, (gGattCharPropRead_c))
		VALUE(value_char_pid, MIBLE_CHAR_UUID_PRODUCTID, (gPermissionFlagReadable_c), MIBLE_CHAR_LEN_PRODUCTID, 0x00)
	CHARACTERISTIC(mi_char_version,MIBLE_CHAR_UUID_VERSION, (gGattCharPropRead_c))
		VALUE(value_char_version, MIBLE_CHAR_UUID_VERSION, (gPermissionFlagReadable_c), MIBLE_CHAR_LEN_VERSION, 0x00)
	CHARACTERISTIC(mi_char_wificfg, MIBLE_CHAR_UUID_WIFICFG, (gGattCharPropNotify_c | gGattCharPropWrite_c))
		VALUE(value_char_wificfg, MIBLE_CHAR_UUID_WIFICFG, (gPermissionFlagWritable_c), MIBLE_CHAR_LEN_WIFICFG, 0x00)
		CCCD(cccd_char_wificfg)
	CHARACTERISTIC(mi_char_authen, MIBLE_CHAR_UUID_AUTHENTICATION, (gGattCharPropWrite_c))
		VALUE(value_char_authen, MIBLE_CHAR_UUID_AUTHENTICATION, (gPermissionFlagWritable_c), MIBLE_CHAR_LEN_AUTHENTICATION, 0x00)
	CHARACTERISTIC(mi_char_did, MIBLE_CHAR_UUID_DID, (gGattCharPropWrite_c | gGattCharPropRead_c))
		VALUE(value_char_did, MIBLE_CHAR_UUID_DID, (gPermissionFlagWritable_c | gPermissionFlagReadable_c), MIBLE_CHAR_LEN_DID, 0x00)
	CHARACTERISTIC(mi_char_beaconkey, MIBLE_CHAR_UUID_BEACONKEY, (gGattCharPropRead_c))
		VALUE(value_char_beaconkey, MIBLE_CHAR_UUID_BEACONKEY, (gPermissionFlagReadable_c), MIBLE_CHAR_LEN_BEACONKEY, 0x00)
	
