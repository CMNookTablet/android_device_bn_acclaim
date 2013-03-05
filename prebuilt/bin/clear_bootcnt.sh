#!/system/bin/sh
##################################################################################
#
# File          clear_bootcount.sh
# Description	Clear the bootcount variable to 0 on successful boot
#
##

# Zero the boot count
dd if=/dev/zero of=/bootdata/BootCnt bs=1 count=4
