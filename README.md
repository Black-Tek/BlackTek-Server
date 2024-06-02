Black Tek Server
===============

Black Tek Server is a free and open-source MMORPG server emulator written in C++. It is a fork of the [TheForgottenServer](https://github.com/otland/forgottenserver) project. To connect to the server, you can use [OTClient Redemption](https://github.com/mehah/otclient).

### Getting Started
If you don't want to bother with source code or compiling. You can download from our release sections the binaries with everything needed ready to go.

#### Compiling 
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

Next you will need to enable manifest mode for vcpkg.

You can do so by enabling the environment variable ```VCPKG_FEATURE_FLAGS=manifests```

Alternatively, for visual studio users, you can just go to ```project->properties->configuration properties->vcpkg``` and change ```Use Vcpkg Manifest``` to ```yes```.

You can find more information about manifest mode [here.](https://learn.microsoft.com/en-us/vcpkg/concepts/manifest-mode)

Once you have enabled manifest mode, you can now select your configuration, and run "Rebuild all" (or its equivalent) to produce your executable.

### Support
### Issues
