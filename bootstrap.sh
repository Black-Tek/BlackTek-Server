#!/bin/bash

# Echo tags for coloring:
RED="\033[0;31m"
GREEN="\033[0;32m"
ORANGE="\033[31;33m"
END="\033[0;0m"

#Premake standard path (the path can be changed and arguments added):
premake_cmd="${HOME}/.local/bin/premake5"
premake_args=""

# Prerequisite packages:
debian_install="apt-get -y install git zip unzip tar curl build-essential uuid-dev"
arch_install="pacman -Syu git zip"
fedora_install="dnf -y install git make gcc-c++ libuuid-devel perl-IPC-Cmd"
suse_install="zypper -y install git-core make gcc-c++ libuuid-devel"

# Read kernel architecture (and version):
	# Posible values: https://stackoverflow.com/questions/45125516/possible-values-for-uname-m .
arch=$(uname -m)

build_arch="64"
make_debug="make -j \`nproc\` config=debug_${build_arch}"
make_release="make -j \`nproc\` config=release_${build_arch}"

# Commands for specific architectures:
case "$arch" in
	aarch64 | armv8* | armv9* | arm64)
		build_arch="arm64"
		# Guide for Docker compilation + ARM64 configuration for premake5 and
		# troubleshooting: https://github.com/Black-Tek/BlackTek-Server/pull/5 .
		export VCPKG_FORCE_SYSTEM_BINARIES=1
		debian_install="${debian_install} cmake ninja-build"
		arch_install="${arch_install} cmake ninja"
		fedora_install="${fedora_install} cmake ninja-build"
		suse_install="${suse_install} cmake ninja-build"
		;;
	arm | armv7*)
		build_arch="arm"
		export VCPKG_FORCE_SYSTEM_BINARIES=1
		premake_args="${premake_args} --lua=lua5.4"
		# Only works on Debian 12 and Ubuntu 24.04, older versions have to git clone tomlplusplus.
		debian_install="${debian_install} liblua5.4-dev libmysqlclient-dev libboost-system-dev libboost-iostreams-dev libpugixml-dev libcrypto++-dev libfmt-dev libcurl4-openssl-dev libtomlplusplus-dev"
		skip_vcpkg=true
		echo "${ORANGE}Arm support is experimental and will likely require some manual setup.${END}"
		read -p "Press [Enter] to continue or [Ctrl + C] to cancel." _
		;;
	armv6* | i386 | i686)
		echo "${RED}=== 32 bit systems are not supported ===${END}"
		exit 1
		;;
	x86_64)
		build_arch="64"
		;;
	*)
		echo "${ORANGE}Unknown processor architecture ${arch}. If your OS is 64 bit it may still work."
		echo "32 bit systems are not supported.${END}"
		read -p "Press [Enter] to continue or [Ctrl + C] to cancel." _
		;;
esac

# Check if dependencies are met:
check_deps () {
	if ! command -v "git" &> /dev/null \
	|| ! command -v "make" &> /dev/null \
	|| ! command -v "zip" &> /dev/null \
	|| ! command -v "curl" &> /dev/null \
	|| ( command -v "pkg-config" &> /dev/null && ! pkg-config -xists uuid &> /dev/null ); then
		return 1
	fi

	if [ "$build_arch" = "ARM64" ] && ( ! command -v "cmake" &> /dev/null || ! command -v "ninja" &> /dev/null ); then
		return 1
	fi

	return 0
}

# Read the Gnu Compiler Collection version:
gcc_version=`gcc --version | awk 'NR==1 {print $3}' | cut -f 1 -d "."`

# Check if the Gnu Compiler Collection is too old:
if [ ${gcc_version} -lt 10 ]
then
	echo "${RED}=== GCC version 10 or above is required. ===${END}"
	exit 1
fi

# Automatically install missing dependencies:
if ! check_deps then
	echo "Some required software could not be found."
	read -p "Attempt automatic install? (y/n): " do_install
	if [ ! $do_install ] || [ $do_install = "n" ]
	then
		# Print installation commands if automatic installation is not possible:
		echo "Please install the prerequisite software using your package manager."
		echo
		echo "Debian/Ubuntu: ${debian_install}"
		echo "Arch: ${arch_install}"
		echo "Fedora: ${fedora_install}"
		echo "OpenSUSE: ${suse_install}"
		exit 1
	fi

	# Check if Advance Package Manager (on Debian, Ubutu or similar distro) is installed for automated dependencies install:
	if command -v "apt" &> /dev/null
		then
		echo "${GREEN}Installing required software${END}"
		sudo apt-get -y -q update
		if ! sudo $debian_install
		then
			echo "Missing dependencies were not installed, check for"
			echo "missing repositories or packages on your distro."
			echo "${RED}=== Configuration is not finished ===${END}"
			exit 1
		fi

	# Check if Pacman (on Arch or similar distro) is installed for automated dependencies install:
	elif command -v "pacman" &> /dev/null
	then
		echo "${GREEN}Installing required software${END}"
		if ! sudo $arch_install
		then
			echo "Missing dependencies were not installed, check for"
			echo "missing repositories or packages on your distro."
			echo "${RED}=== Configuration is not finished ===${END}"
			exit 1
		fi

	# Check if Dandified Yum (on Fedora, Oracle, RHEL or similar distro) is installed for automated dependencies install:
	elif command -v "dnf" &> /dev/null
	then
		echo  "${GREEN}Installing required software${END}"
		if ! sudo $fedora_install
		then
			echo "Missing dependencies were not installed, check for missing repositories."
			echo "${RED}=== Configuration is not finished ===${END}"
			exit 1
		fi

	# Check if Zipper (on SuSE or similar distro) is installed for automated dependencies install:
	elif command -v "zypper" &> /dev/null
	then
		echo  "${GREEN}Installing required software${END}"
		if ! sudo $suse_install
		then
			echo "Missing dependencies were not installed, check for missing repositories."
			echo "${RED}=== Configuration is not finished ===${END}"
			exit 1
		fi

	# Print manual dependencies install commands:
	else
		echo "Automatic install is not supported on your distribution."
		echo "Please install the prerequisite software using your package manager."
		echo
		echo "Debian/Ubuntu: ${debian_install}"
		echo "Arch: ${arch_install}"
		echo "Fedora: ${fedora_install}"
		echo "OpenSUSE: ${suse_install}"
		echo
		echo "${RED}=== Configuration is not finished ===${END}"
		exit 1
	fi
then

# Save BlackTek-Server path:
dir=`pwd`

# Create essential folder:
if [ ! -d ~/.local/bin ]
then
	mkdir -p ~/.local/bin
fi

# Install/Build and update Premake build configurator:
if ! command -v "${premake_cmd}" &> /dev/null
then
	echo "${GREEN}Installing premake${END}"
	already_uptodate=falsedir
	# Check if Premake is installed and up to date:
	if [ ! -d ../premake-core ]
	then
		git clone https://github.com/premake/premake-core ../premake-core
		cd ../premake-core
	else
		cd ../premake-core
		if [ "$(git pull --ff-only)" = "Already up to date." ]
		then
			already_uptodate=true
		fi
	fi
	# Compile and update Premake:
	if [ ! $already_uptodate ] || [ ! -f bin/release/premake5 ]
	then
		if make -f Bootstrap.mak linux; then
			cp -f bin/release/premake5 ~/.local/bin/premake5
		else
			echo "${RED}=== An error occured while compiling premake. Configuration is not complete. ===${END}"
			exit 1
		fi
	fi
fi

# Use (or not) VCPKG:
if [ ! $skip_vcpkg ]
then
	# Check for VCPKG in enviroment variables:
	if ! printenv VCPKG_ROOT &> /dev/null
	then
		if [ -d $HOME/vcpkg ]
		then
			export VCPKG_ROOT=$HOME/vcpkg
		else
			export VCPKG_ROOT=../vcpkg
		fi
	fi
	# Donwload VCPKG:
	if [ ! -d $VCPKG_ROOT ] || [ ! -f $VCPKG_ROOT/vcpkg ]
	then
		echo "${GREEN}Installing vcpkg${END}"
		if [ ! -d $VCPKG_ROOT ]
		then
			git clone https://github.com/microsoft/vcpkg ../vcpkg
		fi
		cd $VCPKG_ROOT
		if ! ./bootstrap-vcpkg.sh -disableMetrics
		then
			echo "${RED}=== An error occured while installing vcpkg. Configuration is not complete. ===${END}"
			exit 1
		fi
		cp -f ./vcpkg ~/.local/bin/vcpkg
	fi
fi

# Go to BlackTek-Server path:
cd $dir

# In case .local/bin is not in path we use the path where we installed it instead:
if ! ${premake_cmd} gmake2 ${premake_args}
then
	echo "${RED}=== An error occured while executing premake. Configuration is not complete. ===${END}"
	exit 1
fi

# VCPKG not found:
if [ ! $skip_vcpkg ]
then
	if ! $VCPKG_ROOT/vcpkg install
	then
		echo "${RED}=== An error occured while executing vcpkg. Configuration is not complete. ===${END}"
		exit 1
	fi
fi

# Build type selection:
echo "${GREEN}=== Configuration Finished ===${END}"
echo
read -p "Would you like to compile the server now? (n: No, d: Debug, r: Release) " compile

# Stop script and print build commands:
if ([ ! $compile ] || [ $compile = "n" ])
then
	echo "To compile the server run:"
	echo "Debug: ${make_debug}"
	echo "Release: ${make_release}"
	exit 0
fi

# Build bebug version:
elif [ $compile = "d" ]
then
	if make -j `nproc` "config=debug_${build_arch}"
	then
		echo
		echo "${GREEN}=== Compilation Successful ===${END}"
		echo
		echo "To start the server: ./Black-Tek-Server"
		echo "Remember to configure MySQL details in config.lua!"
		echo
		echo "To re-compile:"
		echo "Debug: ${make_debug}"
		echo "Release: ${make_release}"
		echo "Alternatively re-run bootstrap.sh"
		exit 0
	else
		echo
		echo "${RED}=== Compilation Failed ===${END}"
		exit 1
	fi
	
# Build realease version:
elif [ $compile = "r" ]
then
	if make -j `nproc` "config=release_${build_arch}"
	then
		echo
		echo "${GREEN}=== Compilation Successful ===${END}"
		echo
		echo "To start the server: ./Black-Tek-Server"
		echo "Remember to configure MySQL details in config.lua!"
		echo
		echo "To re-compile:"
		echo "Debug: ${make_debug}"
		echo "Release: ${make_release}"
		echo "Alternatively re-run bootstrap.sh"
		exit 0
	else
		echo
		echo "${RED}=== Compilation Failed ===${END}"
		exit 1
	fi
fi
