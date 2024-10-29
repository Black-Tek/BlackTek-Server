<div align="center">

<img src="https://miro.medium.com/v2/resize:fit:1024/0*tTUoVKGzxtZYA5xp.png" align="center" width="144px" height="144px"/>

### Mythbound Server Engine

_... built on BlackTek & TFS 1.42_ 🤖

</div>

<div align="center">

[![Kubernetes](https://img.shields.io/static/v1?label=K3s&message=v1.30.2&color=blue&style=for-the-badge&logo=kubernetes&logoColor=white)](https://k3s.io/)&nbsp;&nbsp;
[![Discord](https://img.shields.io/discord/1283279739775352896?style=for-the-badge&label&logo=discord&logoColor=white&color=blue)]([https://discord.gg/1283279739775352896](https://discord.gg/Erhz4GmDMd))&nbsp;&nbsp;
[![Renovate](https://img.shields.io/github/actions/workflow/status/tibia-oce/infrastructure/renovate.yaml?branch=master&label=&logo=renovatebot&style=for-the-badge&color=blue)](https://github.com/tibia-oce/infrastructure/actions/workflows/renovate.yaml)

</div>

<div align="center">

[![Cluster](https://img.shields.io/static/v1?label=Cluster&message=Online&color=brightgreen&style=for-the-badge&logo=v&logoColor=white)](https://status.mythbound.dev/)&nbsp;&nbsp;
[![Server](https://img.shields.io/static/v1?label=Server&message=Online&color=brightgreen&style=for-the-badge&logo=v&logoColor=white)](https://status.mythbound.dev/endpoints/tibia_tibia-server)&nbsp;&nbsp;
[![Website](https://img.shields.io/static/v1?label=Website&message=Online&color=brightgreen&style=for-the-badge&logo=statuspage&logoColor=white)](https://myaac.mythbound.dev/)&nbsp;&nbsp;

</div>

<br>

---

<br>



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
