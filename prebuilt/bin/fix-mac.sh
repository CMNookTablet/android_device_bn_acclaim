#!/system/bin/sh

PATH=/sbin:/vendor/bin:/system/sbin:/system/bin:/system/xbin
NVS_BIN=/system/etc/firmware/ti-connectivity/wl1271-nvs.bin

if [ ! -f "$NVS_BIN" ]; then
busybox mount -o remount,rw /system
    cp ${NVS_BIN}.orig ${NVS_BIN}
    calibrator set nvs_mac $NVS_BIN F0:A2:25:$(dmesg|grep -i die-id|cut -c43-|sed -e 's/)//g'|sed -e 's/-//g'|sed 's/../&:/g;s/:$//')
    busybox mount -o remount,ro /system
fi