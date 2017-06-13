#!/system/bin/sh

export PATH=/data/busybox:$PATH
sleep 1

#wlanmod=`lsmod`
#case "$wlanmod" in
#    *wlan*)
#        echo "WLAN module detected"
#        rmmod wlan
#        sleep 2
#        /system/bin/insmod /system/lib/modules/wlan.ko
#        sleep 1
#        ;;
#    *)
#        echo "No WLAN module detected"
#        /system/bin/insmod /system/lib/modules/wlan.ko
#        sleep 1
#        ;;
#esac
setprop "wlan.driver.status" "ok"

startwpasupp.sh
#/system/bin/wpa_supplicant -iwlan0 -Dnl80211 -dddd -c/data/misc/wifi/wpa_supplicant.conf &
#sleep 1

wfa_dut lo 8000
