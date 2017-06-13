The toolbox that is provided on the Android environment is pretty limited. I wanted something more useful and familiar. Busybox to the rescue!
I have a busybox binary available for those who are interested.
To use it simply do # mkdir /data/busybox, on your emulated console. Then copy the busybox binary across: $ adb push busybox /data/busybox/busybox. Once you do this you can install the necessary hardlinks by doing: # cd /data/busybox; ./busybox --install. Once doing this you should have links to all the applets in the /data/busybox directory. To make this useful you probably want to put this into your PATH: # export PATH=/data/busybox:$PATH.
You should now be able to get access to all the useful busybox applets.
Update:busybox was a straight compile of the standard busybox 1.8.1 release. No changes were made to the source. Source code available here or from the Busybox website.
