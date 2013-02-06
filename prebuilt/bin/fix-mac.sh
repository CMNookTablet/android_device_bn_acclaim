#!/system/bin/sh


PATH=/sbin:/vendor/bin:/system/sbin:/system/bin:/system/xbin
ORIG_NVS_BIN=/system/etc/firmware/ti-connectivity/wl1271-nvs.bin.orig
NVS_BIN=/system/etc/firmware/ti-connectivity/wl1271-nvs.bin

busybox mount -o remount,rw /system

if busybox [ ! -f "$NVS_BIN" ]; then
    cp ${ORIG_NVS_BIN} ${NVS_BIN}
fi

MAC=`cat /rom/devconf/MACAddress | sed 's/\(..\)\(..\)\(..\)\(..\)\(..\)/\1:\2:\3:\4:\5:/'`
calibrator set nvs_mac $NVS_BIN $MAC

busybox mount -o remount,ro /system