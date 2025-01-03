#!/bin/bash

# Echo tags
RED="\033[0;31m"
GREEN="\033[0;32m"
ORANGE="\033[31;33m"
END="\033[0;0m"

premake_cmd="${HOME}/.local/bin/premake5"
premake_args=""

# Prerequisite packages
debian_install="apt-get -y install git zip unzip tar curl build-essential uuid-dev pkg-config"
arch_install="pacman -Syu git zip"
fedora_install="dnf -y install git make gcc-c++ libuuid-devel perl-IPC-Cmd"
suse_install="zypper -y install git-core make gcc-c++ libuuid-devel"

# https://stackoverflow.com/questions/45125516/possible-values-for-uname-m
arch=$(uname -m)

build_arch="64"
make_debug="make -j \`nproc\` config=debug_${build_arch}"
make_release="make -j \`nproc\` config=release_${build_arch}"

case "$arch" in
	aarch64 | armv8* | armv9* | arm64)
		build_arch="arm64"
		# https://github.com/Black-Tek/BlackTek-Server/pull/5
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
		# Only works on Debian 12 and Ubuntu 24.04, older versions have to git clone tomlplusplus
		debian_install="${debian_install} liblua5.4-dev libmysqlclient-dev libboost-system-dev libboost-iostreams-dev libpugixml-dev libcrypto++-dev libfmt-dev libcurl4-openssl-dev libtomlplusplus-dev"
		skip_vcpkg=true
		echo -e "${ORANGE}Arm support is experimental and will likely require some manual setup.${END}"
		read -p "Press [Enter] to continue or [Ctrl + C] to cancel." _
		;;
	armv6* | i386 | i686)
		echo -e "${RED}=== 32 bit systems are not supported ===${END}"
		exit 1
		;;
	x86_64)
		build_arch="64"
		;;
	*)
		echo -e "${ORANGE}Unknown processor architecture ${arch}. If your OS is 64 bit it may still work."
		echo -e "32 bit systems are not supported.${END}"
		read -p "Press [Enter] to continue or [Ctrl + C] to cancel." _
		;;
esac

check_deps () {
	if ! command -v "git" &> /dev/null \
	|| ! command -v "make" &> /dev/null \
	|| ! command -v "zip" &> /dev/null \
	|| ! command -v "curl" &> /dev/null \
	|| ( command -v "pkg-config" &> /dev/null && ! pkg-config --exists uuid &> /dev/null ); then
		return 1
	fi

	if [ "$build_arch" == "ARM64" ] && ( ! command -v "cmake" &> /dev/null || ! command -v "ninja" &> /dev/null ); then
		return 1
	fi

	return 0
}

gcc_version=`gcc --version | awk 'NR==1 {print $3}' | cut -f 1 -d "."`

if (( gcc_version < 10 )); then
	echo -e "${RED}=== GCC version 10 or above is required. ===${END}"
	exit 1
fi

if ! check_deps; then
	echo "Some required software could not be found."
	read -p "Attempt automatic install? (y/n): " do_install
	if [ ! $do_install ] || [ $do_install == "n" ]; then
		echo "Please install the prerequisite software using your package manager."
		echo
		echo "Debian/Ubuntu: ${debian_install}"
		echo "Arch: ${arch_install}"
		echo "Fedora: ${fedora_install}"
		echo "OpenSUSE: ${suse_install}"
		exit 1
	fi

	if command -v "apt" &> /dev/null; then
		echo -e ${GREEN}Installing required software${END}
		sudo apt-get -y -q update
		if ! sudo $debian_install; then
			echo "Missing dependencies were not installed."
			echo -e "${RED}=== Configuration is not finished ===${END}"
			exit 1
		fi
	elif command -v "pacman" &> /dev/null; then
		echo -e ${GREEN}Installing required software${END}
		if ! sudo $arch_install; then
			echo "Missing dependencies were not installed."
			echo -e "${RED}=== Configuration is not finished ===${END}"
			exit 1
		fi
	elif command -v "dnf" &> /dev/null; then
		echo -e ${GREEN}Installing required software${END}
		if ! sudo $fedora_install; then
			echo "Missing dependencies were not installed."
			echo -e "${RED}=== Configuration is not finished ===${END}"
			exit 1
		fi
	elif command -v "zypper" &> /dev/null; then
		echo -e ${GREEN}Installing required software${END}
		if ! sudo $suse_install; then
			echo "Missing dependencies were not installed."
			echo -e "${RED}=== Configuration is not finished ===${END}"
			exit 1
		fi
	else
		echo "Automatic install is not supported on your distribution."
		echo "Please install the prerequisite software using your package manager."
		echo
		echo "Debian/Ubuntu: ${debian_install}"
		echo "Arch: ${arch_install}"
		echo "Fedora: ${fedora_install}"
		echo "OpenSUSE: ${suse_install}"
		echo
		echo -e "${RED}=== Configuration is not finished ===${END}"
		exit 1
	fi
fi

dir=`pwd`

if [ ! -d ~/.local/bin ]; then
	mkdir -p ~/.local/bin
fi

if ! command -v "${premake_cmd}" &> /dev/null; then
	echo -e "${GREEN}Installing premake${END}"
	already_uptodate=false
	if [ ! -d ../premake-core ]; then
		git clone https://github.com/premake/premake-core ../premake-core
		cd ../premake-core
	else
		cd ../premake-core
		if [ "$(git pull --ff-only)" == "Already up to date." ]; then
			already_uptodate=true
		fi
	fi

	if [ ! $already_uptodate ] || [ ! -f bin/release/premake5 ]; then
		if make -f Bootstrap.mak linux; then
			cp -f bin/release/premake5 ~/.local/bin/premake5
		else
			echo -e "${RED}=== An error occured while compiling premake. Configuration is not complete. ===${END}"
			exit 1
		fi
	fi
fi

if [ ! $skip_vcpkg ]; then
	# Check for vcpkg in env
	if ! printenv VCPKG_ROOT &> /dev/null; then
		if [ -d $HOME/vcpkg ]; then
			export VCPKG_ROOT=$HOME/vcpkg
		else
			export VCPKG_ROOT=../vcpkg
		fi
	fi

	if [ ! -d $VCPKG_ROOT ] || [ ! -f $VCPKG_ROOT/vcpkg ]; then
		echo -e "${GREEN}Installing vcpkg${END}"
		if [ ! -d $VCPKG_ROOT ]; then
			git clone https://github.com/microsoft/vcpkg ../vcpkg
		fi
		cd $VCPKG_ROOT
		if ! ./bootstrap-vcpkg.sh -disableMetrics; then
			echo -e "${RED}=== An error occured while installing vcpkg. Configuration is not complete. ===${END}"
			exit 1
		fi
		cp -f ./vcpkg ~/.local/bin/vcpkg
	fi
fi

cd $dir

# In case .local/bin is not in path we use the path where we installed it instead
if ! ${premake_cmd} gmake2 ${premake_args}; then
	echo -e "${RED}=== An error occured while executing premake. Configuration is not complete. ===${END}"
	exit 1
fi

if [ ! $skip_vcpkg ]; then
	if ! $VCPKG_ROOT/vcpkg install; then
		echo -e "${RED}=== An error occured while executing vcpkg. Configuration is not complete. ===${END}"
		exit 1
	fi
fi

echo -e "${GREEN}=== Configuration Finished ===${END}"
echo
read -p "Would you like to compile the server now? (n: No, d: Debug, r: Release) " compile

if [ ! $compile ] || [ $compile == "n" ]; then
	echo "To compile the server run:"
	echo "Debug: ${make_debug}"
	echo "Release: ${make_release}"
	exit 0
elif [ $compile == "d" ]; then
	if make -j `nproc` "config=debug_${build_arch}"; then
		echo
		echo -e "${GREEN}=== Compilation Successful ===${END}"
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
		echo -e "${RED}=== Compilation Failed ===${END}"
		exit 1
	fi	
elif [ $compile == "r" ]; then
	if make -j `nproc` "config=release_${build_arch}"; then
		echo
		echo -e "${GREEN}=== Compilation Successful ===${END}"
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
		echo -e "${RED}=== Compilation Failed ===${END}"
		exit 1
	fi
fi
