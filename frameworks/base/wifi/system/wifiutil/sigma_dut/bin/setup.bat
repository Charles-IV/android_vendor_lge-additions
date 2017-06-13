adb push wfa_dut /system/bin/
adb shell chmod 755 /system/bin/wfa_dut

adb push dutinit.sh /system/bin/
adb shell chmod 755 /system/bin/dutinit.sh

adb push startdhcp.sh /system/bin/
adb shell chmod 755 /system/bin/startdhcp.sh

adb push startwpasupp.sh /system/bin/
adb shell chmod 755 /system/bin/startwpasupp.sh

adb push wfainit.sh /system/bin/
adb shell chmod 755 /system/bin/wfainit.sh

adb push setbusybox.sh /system/bin/
adb shell chmod 755 /system/bin/setbusybox.sh

adb push getipconfig.sh /system/bin/
adb shell chmod 755 /system/bin/getipconfig.sh

adb push wpa_cli /system/bin/
adb shell chmod 755 /system/bin/wpa_cli

adb push wfaping.sh /system/bin/
adb shell chmod 755 /system/bin/wfaping.sh

adb push updatepid.sh /system/bin/
adb shell chmod 755 /system/bin/updatepid.sh

adb push getpstats.sh /system/bin/
adb shell chmod 755 /system/bin/getpstats.sh

adb push gpstats.sh /system/bin/
adb shell chmod 755 /system/bin/gpstats.sh

adb push stoping.sh /system/bin/
adb shell chmod 755 /system/bin/stoping.sh

adb push getpid.sh /system/bin/
adb shell chmod 755 /system/bin/getpid.sh

adb shell mkdir /data/busybox
adb push busybox /data/busybox/
adb shell chmod 755 /data/busybox/busybox
adb shell /data/busybox/busybox --install

pause
