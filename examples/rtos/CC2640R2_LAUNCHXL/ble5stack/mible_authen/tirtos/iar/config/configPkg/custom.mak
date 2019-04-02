## THIS IS A GENERATED FILE -- DO NOT EDIT
.configuro: .libraries,rm3 linker.cmd package/cfg/app_ble_prm3.orm3

# To simplify configuro usage in makefiles:
#     o create a generic linker command file name 
#     o set modification times of compiler.opt* files to be greater than
#       or equal to the generated config header
#
linker.cmd: package/cfg/app_ble_prm3.xdl
	$(SED) 's"^\"\(package/cfg/app_ble_prm3cfg.cmd\)\"$""\"C:/ti/simplelink_cc2640r2_sdk_2_40_00_32/examples/rtos/CC2640R2_LAUNCHXL/ble5stack/mible_authen/tirtos/iar/config/configPkg/\1\""' package/cfg/app_ble_prm3.xdl > $@
	-$(SETDATE) -r:max package/cfg/app_ble_prm3.h compiler.opt compiler.opt.defs
