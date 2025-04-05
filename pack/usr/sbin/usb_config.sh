##########################################################################
# File Name: usb_config.sh
# Description: Set Device USB mode.
# Author: songliuyang
# Mail: songliuyang@xmsilicon.cn
# Created Time: Thu 09 Feb 2023 01:39:48 PM CST
#########################################################################
#!/bin/sh
mod_dir=/usr/lib/modules
cmd_dir=/sbin

wifi_mod="8188fu cfg80211 wifi_pdn xm_usb0 dwc2"
wifi_ko="dwc2.ko xm_usb0.ko wifi_pdn.ko cfg80211.ko 8188fu.ko"
eth_mod="g_ether libcomposite dwc_otg dwc_common_port_lib"
eth_ko="dwc_common_port_lib.ko dwc_otg.ko libcomposite.ko g_ether.ko"
ms_mod="g_mass_storage libcomposite dwc_otg dwc_common_port_lib"
ms_ko="dwc_common_port_lib.ko dwc_otg.ko libcomposite.ko"
usbc_mod="usbc"

###############################################################################
# WIFI
entry_wifi_mode()
{
	if [ -n "`$cmd_dir/lsmod | grep wifi`" ];then
		echo "Already in wifi mode."
		return 0
	fi

	# close usb0
	if [ -n "`$cmd_dir/ifconfig -a | grep usb0`" ];then
		$cmd_dir/ifconfig usb0 down
	fi
	sleep 1

	# remove eth & ms modules
	for i in $eth_mod $ms_mod; do
		if [ -n "`$cmd_dir/lsmod | grep $i`" ];then
			/sbin/rmmod $i
		fi
	done
	
	regs 0x10020220 0x400	# USB select wifi
	regs 0x10020218 0x400	# USB set HOST mode
	# insert wifi modules
	for i in $wifi_ko; do
		/sbin/insmod $mod_dir/$i
	done
}

exit_wifi_mode()
{
	# remove wifi modules
	for i in $wifi_mod; do
		if [ -n "`$cmd_dir/lsmod | grep $i`" ]; then
			/sbin/rmmod $i
		fi
	done

}


###############################################################################
# RNDIS
entry_rndis_mode()
{
	if [ -n "`$cmd_dir/lsmod | grep rndis`" ];then
		echo "Already in mass storage mode."
		return 0
	fi
	
	# remove wifi & ms modules
	for i in $usbc_mod $wifi_mod $ms_mod; do
		if [ -n "`$cmd_dir/lsmod | grep $i`" ]; then
			/sbin/rmmod $i
		fi
	done
	
	regs 0x10020220 0xC00	# USB select device
	regs 0x10020218 0xC00	# USB set DEVICE mode
	# insert ether modules
	for i in $eth_ko; do
		/sbin/insmod $mod_dir/$i
	done
	if [ -n "`$cmd_dir/ifconfig -a | grep usb0`" ];then
		$cmd_dir/ifconfig usb0 192.168.2.10 netmask 255.255.255.0
		$cmd_dir/route add default gw 192.168.2.1
	else
		echo "Config RNDIS error."
	fi
}

exit_rndis_mode()
{
	# close usb0 interface
	if [ -n "`$cmd_dir/ifconfig -a | grep usb0`" ];then
		$cmd_dir/ifconfig usb0 down
	fi

	# remove ether modules
	for i in $eth_mod; do
		if [ -n "`$cmd_dir/lsmod | grep $i`" ]; then
			/sbin/rmmod $i
		fi
	done

}

###############################################################################
# mass storage
entry_mass_storage_mode()
{
	if [ -n "`$cmd_dir/lsmod | grep mass`" ];then
		echo "Already in mass storage mode."
		return 0
	fi

	# close usb0
	if [ -n "`$cmd_dir/ifconfig -a | grep usb0`" ];then
		$cmd_dir/ifconfig usb0 down
	fi
	sleep 1

	# remove wifi & ether modules
	for i in $usbc_mod $wifi_mod $eth_mod; do
		if [ -n "`$cmd_dir/lsmod | grep $i`" ]; then
			/sbin/rmmod $i
		fi
	done

	# insert wifi modules
	regs 0x10020220 0xC00	# USB select device
	regs 0x10020218 0xC00	# USB set Device mode
    
    if [ -n "`ls /dev | grep mmcblk0`" ];then
        for i in $ms_ko; do
            /sbin/insmod $mod_dir/$i
        done
        if [ -n "`ls /dev | grep mmcblk0p1`" ]; then
            /sbin/insmod $mod_dir/g_mass_storage.ko file=/dev/mmcblk0p1 luns=1 stall=0 removable=1
        else
            /sbin/insmod $mod_dir/g_mass_storage.ko file=/dev/mmcblk0 luns=1 stall=0 removable=1
        fi
    else
        echo "NO SD card."
    fi
}

exit_mass_storage_mode()
{
	# remove other modules
	for i in $ms_mod ; do
		if [ -n "`$cmd_dir/lsmod | grep $i`" ]; then
			/sbin/rmmod $i
		fi
	done
}

usage_exit()
{
	echo "usage: $1 [option] [mode]"
	echo "  option    -i enable USB funciton"
	echo "            -r disable USB funciton"
	echo ""
	echo "  mode      0: wifi mode"
	echo "            1: rndis mode"
	echo "            2: mass storage mode"
	echo ""
	exit
}

opt=$1
mod=$2
if [ "$opt" = "-i" ];then
	if [ $mod -eq 0 ];then				## wifi mode
		entry_wifi_mode
	elif [ $mod -eq 1 ];then            ## ether mode
		entry_rndis_mode
	elif [ $mod -eq 2 ];then            ## mass storage mode
		entry_mass_storage_mode
	else
		usage_exit $0
	fi
elif [ "$opt" = "-r" ];then
	if [ $mod -eq 0 ];then				## wifi mode
		exit_wifi_mode
	elif [ $mod -eq 1 ];then            ## ether mode
		exit_rndis_mode
	elif [ $mod -eq 2 ];then            ## mass storage mode
		exit_mass_storage_mode
	else
		usage_exit $0
	fi
else
	usage_exit $0
fi

