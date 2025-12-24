# Black Tek Server 
__________________
[![Discord Shield](https://discordapp.com/api/guilds/1251683017441677372/widget.png?style=shield)](https://discord.gg/dy5wXSzbPG)
[![Linux Build](https://github.com/Black-Tek/BlackTek-Server/actions/workflows/linux_build_runner.yml/badge.svg?branch=master)](https://github.com/Black-Tek/BlackTek-Server/actions/workflows/linux_build_runner.yml) 
[![Windows Build](https://github.com/Black-Tek/BlackTek-Server/actions/workflows/windows_build_runner.yml/badge.svg)](https://github.com/Black-Tek/BlackTek-Server/actions/workflows/windows_build_runner.yml)

## What is BlackTek Server?
__________________
**BlackTek Server** is an open source **2D Top Down MMORPG Game Server**, with tailor-made gameplay and tile based movement,  developed in modern C++. 

## What is the point of BlackTek Server?
_______________
**BlackTek Server** is intended to provide a user-friendly experience building 2D MMORPG's in a _**rapid development environment**_. Ultimately the goal is to create a user experience that removes the barrier between _**content creator**_ and _**programmer**_. 

## What sparked this idea?
__________________
BlackTek Server's origins started first with [OpenTibia](https://github.com/opentibia/server) and later [The Forgottenserver](https://github.com/otland/forgottenserver), both are game servers designed to emulate a popular 2D MMORPG known as Tibia.

Having had my fun growing up tinkering with OpenTibia servers like TFS, I always wished they were not built so strict (explicitly emulating tibia), and allowed much more custom types of things. Since that never happened, I decided to do it myself!

BlackTek Server's starting codebase is [The Forgottenserver 1.4.2](https://github.com/otland/forgottenserver/releases/tag/v1.4.2), and BlackTek Server [1.0]() and [1.1]() were built to be almost completely backwards compatible with TFS 1.4.2.

## BlackTek Goals
If you would like to get an understanding for the future plans of BlackTek Server, take a look [here](https://github.com/Black-Tek/BlackTek-Server/wiki/Official-To%E2%80%90Do-List)

## Contributing
I haven't created official contribution guidelines just yet, but any bugs found and reported is very helpful. You can also support this project financially through the discord.

## Getting Started
____________
If you wish to get started immediately you may download all the binaries along with the datapack and other required files from our [Release Section](https://github.com/Black-Tek/BlackTek-Server/releases).

Getting setup for compiling can be done the easy way by running either the ```bootstrap.bat```(Windows) or ```bootstrap.sh```(Linux).

If you prefer compiling manually, or you are looking for a more thorough getting started guide, you can find the information needed, based on your specific needs in our wiki [here](https://github.com/Black-Tek/BlackTek-Server/wiki/Getting-Started#compiling).

### Running with Docker

You can run BlackTek Server using Docker on Windows, Linux or macOS:

```bash
docker-compose up -d --build
```

This will compile your `/src` sources, start MariaDB, and run the game server on ports 7171/7172. Any changes you make to the source code will be compiled when you run with `--build`. You can manage your database through [whodb](https://github.com/clidey/whodb) at `http://localhost:8080`.

> [!NOTE]
> On Windows, if the build fails due to memory issues, increase WSL memory by creating `C:\Users\YOUR_USER\.wslconfig`:
> ```ini
> [wsl2]
> memory=16GB
> processors=8
> swap=4GB
> ```
> Then restart WSL with `wsl --shutdown` and try again.

## Where to find a compatible client?
____________
The BlackTek server is currently using the Tibia 10.98 client protocol. You can use either the original client which you can find [here](https://downloads.ots.me/data/tibia-clients/windows/exe/Tibia1098.exe) as an .exe, or [here](https://downloads.ots.me/data/tibia-clients/windows/zip/Tibia1098.zip) as a .zip. 

Alternatives which support this protocol include [Open Tibia Client](https://github.com/edubart/otclient) and any of it's derivatives such as [OTC Redemption](https://github.com/mehah/otclient#-otclient---redemption) & [OTCv8 (OTA)](https://github.com/OTAcademy/otclientv8).