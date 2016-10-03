#!/bin/bash
#Script written by Umair - http://www.NoobsLab.com
#Run this script as root
if [ $EUID -ne 0 ]; then
   echo "VMware installation script must be run as root. (Hint: use sudo)" 1>&2
   exit 1
fi
echo "
Keep visit on http://www.NoobsLab.com
"
sleep 2
echo "This installation script is only for `printf "\e[32mDebian"``echo -e "\033[0m"`/`printf "\e[32mUbuntu"``echo -e "\033[0m"`/`printf "\e[32mLinux Mint"``echo -e "\033[0m"`/`printf "\e[32mDebian & Ubuntu derivatives"``echo -e "\033[0m"`/`printf "\e[32mUbuntu Flavors"``echo -e "\033[0m"`.
"
read -p "Are you running any of `printf "\e[32mabove"``echo -e "\033[0m"` distribution? (Press Y/y to continue) " -n 1 -r
echo ""
if [[ $REPLY =~ ^[Yy]$ ]]; then
    echo "Okay, installation is starting"
	echo "Installing build tools & linux headers"
	sleep 1
	apt-get install build-essential linux-headers-$(uname -r)
	echo ""
	echo "Checking OS Architecture..."
	sleep 1
	CHKArch=`uname -m`
	sleep 1
    #64bit
    if [ $CHKArch = "x86_64" ]; then
		echo "You are running `printf "\e[32m64bit OS"``echo -e "\033[0m"`."
		echo ""
		sleep 1
		echo "Downloading VMware Player from official VMware Website"
		wget -O VMware-Player-7.1.2-2780323.x86_64.bundle https://download3.vmware.com/software/player/file/VMware-Player-7.1.2-2780323.x86_64.bundle
		echo "Making downloaded file executeable"
		chmod +x VMware-Player-7.1.2-2780323.x86_64.bundle
		echo "Now running VMware setup"
		sleep 1
		./VMware-Player-7.1.2-2780323.x86_64.bundle
		rm VMware-Player-7.1.2-2780323.x86_64.bundle
		exit 1
    #i386
    elif [ $CHKArch = "i686" ] || [ $CHKArch = "i386" ] || [ $CHKArch = "i586" ]; then
		echo "You are running `printf "\e[32m32bit OS"``echo -e "\033[0m"`."
		sleep 1
		echo ""
		echo "Downloading VMware Player from official VMware Website"
		wget -O VMware-Player-6.0.6-2700073.i386.bundle https://download3.vmware.com/software/player/file/VMware-Player-6.0.6-2700073.i386.bundle
		echo "Making downloaded file executeable"
		chmod +x VMware-Player-6.0.6-2700073.i386.bundle
		echo "Now running VMware setup"
		sleep 1
		./VMware-Player-6.0.6-2700073.i386.bundle
		rm VMware-Player-6.0.6-2700073.i386.bundle
		exit 1
    else
		echo "It is not available for your system architecture."
		exit
    fi

elif [[ ! $REPLY =~ ^[Yy]$ ]]; then
    echo "exiting..."
    exit 1
fi
