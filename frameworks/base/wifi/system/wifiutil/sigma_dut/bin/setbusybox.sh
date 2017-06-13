#!/system/bin/sh
export PATH=/data/busybox:$PATH
chmod 755 /data/busybox/busybox
sleep 1
busybox --install
