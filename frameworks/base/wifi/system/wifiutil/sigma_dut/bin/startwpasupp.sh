#!/system/bin/sh

pid_no=`cat "/data/misc/wifi/wpa_supp_pid.txt" | busybox grep PID | busybox cut -f2 -d'='`
#echo pid_no=$pid_no
kill $pid_no

chmod 777 /system/bin/wpa_supplicant
/system/bin/wpa_supplicant -i/system/bin/wlan0 -Dnl80211 -dddd -c/data/misc/wifi/wpa_supplicant.conf &
echo PID=$! > /data/misc/wifi/wpa_supp_pid.txt
sleep 1