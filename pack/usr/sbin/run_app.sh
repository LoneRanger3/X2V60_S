##########################################################################
# File Name: run_app.sh
# Created Time: Mon 30 May 2022 09:54:44 AM CST
#########################################################################
#!/bin/sh

BootSysIndex=`sed -n "s/xmauto:\(.*\).*/\1\n/p" /proc/xm/xminfo`
if [ $BootSysIndex = 1 ];then
    /usr/bin/car_demo &
else
    echo "do not auto start app."
    rmmod xm_wdt
fi

up_time=`cat /proc/uptime`
echo "app start in ${up_time% *}"



