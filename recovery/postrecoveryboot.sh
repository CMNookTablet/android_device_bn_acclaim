#!/sbin/sh

# Resets the boot counter and the BCB instructions
mkdir /bootdata
mount /dev/block/mmcblk0p6 /bootdata
mount -o rw,remount /bootdata

# Zero out the boot counter
dd if=/dev/zero of=/bootdata/BootCnt bs=1 count=4

# Reset the bootloader control block (bcb) file
dd if=/dev/zero of=/bootdata/BCB bs=1 count=1088

umount /bootdata
rmdir /bootdata
