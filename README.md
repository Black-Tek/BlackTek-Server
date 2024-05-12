Black Tek Server
===============

Black Tek Server is a free and open-source MMORPG server emulator written in C++. It is a fork of the [TheForgottenServer](https://github.com/otland/forgottenserver) project. To connect to the server, you can use [OTClient Redemption](https://github.com/mehah/otclient).

### Getting Started
If you don't want to bother with source code or compiling. You can download from our release sections the binaries with everything needed ready to go.

#### Compiling 
To compile using the [default build system](https://premake.github.io/) and [package manager](https://conan.io/) you will need [python](https://www.python.org/) at least version [3.10.14](https://www.python.org/downloads/release/python-31014/) and conan installed. 
Start with python first. If you already have python installed, you can proceed to installing conan with pip in terminal.
Once you have installed python successfully, you need to run the following command to install Conan the package manager:

    pip install conan

You need to make sure you have full rights in the terminal while executing the command.

Window's users should run as administrator.
Note to windows users, toolset 143 latest version for x86_64 (can be found under individual components in visual studio installer) and cmake must both be installed with visual studio as well. 

Linux users need to sudo.
Linux users also need to compile premake themselves. It is included in the vendors folder. It's a simple one source file compile using make is all that is required, instructions for compiling it is included inside its build folder. 
If you are having issues with linux please check our troubleshooting guide [here](). <-- doesn't exist yet.

Once you have successfully installed Conan, you can now run the script "setup_build.py" and wait for it to finish its process. Once it has closed, you will have your project files (in windows they are in folder "projectfiles") for use in compiling. 

If you are using visual studio, you can open your solution, select your preferred configuration, and build. 

For linux users, your makefile's are ready to be used via terminal or your favorite IDE. 

### Support
### Issues
