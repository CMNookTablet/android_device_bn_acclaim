#!/system/bin/sh

if [ -f /rom/devconf/SerialNumber ] ; then
    fsn=$(cat /rom/devconf/SerialNumber)
else
    fsn=$(cat /sys/board_properties/soc/die_id | sed -e 's/-//g; s/^................//')
fi

if [ 16 == ${#fsn} ] ; then
    if [ -x /system/xbin/setpropex ] ; then
	/system/xbin/setpropex ro.serialno ${fsn}
    fi
fi
