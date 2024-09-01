Black Tek Server [![Linux Build](https://github.com/Black-Tek/BlackTek-Server/actions/workflows/linux_build_runner.yml/badge.svg?branch=master)](https://github.com/Black-Tek/BlackTek-Server/actions/workflows/linux_build_runner.yml) [![Windows Build](https://github.com/Black-Tek/BlackTek-Server/actions/workflows/windows_build_runner.yml/badge.svg)](https://github.com/Black-Tek/BlackTek-Server/actions/workflows/windows_build_runner.yml)
===============

Black Tek Server is a free and open-source MMORPG server emulator written in C++. It is a fork of the [TheForgottenServer](https://github.com/otland/forgottenserver) project. To connect to the server, you can use [OTClient Redemption](https://github.com/mehah/otclient).

### Getting Started
If you don't want to bother with source code or compiling. You can download from our release sections the binaries with everything needed ready to go.

#### Compiling 

For windows users looking for the quickest way to get started with BlackTek Server, you can follow the steps provided [here](https://otland.net/threads/tutorial-blacktek-from-nothing-to-fully-installed-with-aac-on-windows.289572/)

Otherwise please follow the steps provided below:

To compile you will need to install and integrate vcpkg.
You can find detailed instructions for [Windows](https://github.com/microsoft/vcpkg?tab=readme-ov-file#quick-start-windows) and [Unix](https://github.com/microsoft/vcpkg?tab=readme-ov-file#quick-start-unix) based operating systems.

Once you have successfully installed vcpkg, and integrated the installation, you will need to download the newest [premake](https://github.com/premake/premake-core.git) from the dev branch (must use dev branch, its the one hyperlinked).

Please follow instructions on how to compile premake for your Operating System. Once you have premake compiled, place the binary in BlackTek-Server's folder. Then run premake via terminal/command line/bash using the following command:

Windows:
```premake5 vs2022```

Linux:
```./premake5 gmake2```

OSX aka Mac:
```premake5 xcode4```

This will generate your project files. 

If you are using linux, please ensure you have manifest mode enabled.
You can find more information about manifest mode [here.](https://learn.microsoft.com/en-us/vcpkg/concepts/manifest-mode)

### Support
### Issues
