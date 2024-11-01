Mythbound Server [![Release](https://github.com/tibia-oce/server/actions/workflows/build-release.yaml/badge.svg?branch=master)](https://github.com/tibia-oce/server/actions/workflows/build-release.yaml)
===============

The Mythbound Server is a free and open-source MMORPG server emulator written in C++. It is a fork of the [BlackTek](https://github.com/Black-Tek/BlackTek-Server) project. To connect to the server, you can use [Mythbound Client](https://github.com/tibia-oce/otclient).

### Getting Started

To build and run it locally from source, all you require is [Docker](https://www.docker.com/) and [Make](https://www.gnu.org/software/make/#download) installed, then from the project root, run `make`.  

The latest release of the Mythbound Server is available via `docker pull ghcr.io/tibia-oce/server:latest`. 


### Compiling 

All compiling is done in Docker containers to streamline the setup process for you. This allows users to rebuild the server source reproducibly with the `make` command. The current container configs support building for **both ARM and AMD architectures** on Linux, Windows and MacOS, making it adaptable for various systems.

**Alternatively**, if you'd prefer to compile the application manually, you will need to install and integrate vcpkg. You can find detailed instructions for [Windows](https://github.com/microsoft/vcpkg?tab=readme-ov-file#quick-start-windows) and [Unix](https://github.com/microsoft/vcpkg?tab=readme-ov-file#quick-start-unix) based operating systems. Once you have successfully installed vcpkg and integrated the installation, download the newest [Premake](https://github.com/premake/premake-core.git).

### Environment Variables

There are defaults set already when you run `make`, however you can adjust the environment variables in the `.env` file as described below:

|           NAME              |               DESCRIPTION                           |       DEFAULT VALUE                   |
| :-------------------------- | :-------------------------------------------------  | :-----------------------------------  |
| `MYSQL_DATABASE`            | The name of the MYSQL database                      | `forgottenserver`                     |
| `MYSQL_HOST`                | The hostname or IP address of the MYSQL server      | `database`                            |
| `MYSQL_PORT`                | The port number MYSQL server is listening on        | `3306`                                |
| `MYSQL_ROOT_PASSWORD`       | The root password for MYSQL                         | `forgottenserver`                     |
| `MYSQL_USERNAME`            | The username for connecting to MYSQL                | `forgottenserver`                     |
| `MYSQL_SOCK`                | The socket file path for local MYSQL connections    | `/var/run/mysqld/mysqld.sock`         |
| `STATUS_PROTOCOL_PORT`      | The port used for the status protocol of the server | `7171`                                |
| `GAME_PROTOCOL_PORT`        | The port used for the game protocol of the server   | `7172`                                |
| `SERVER_NAME`               | The display name of the server                      | `Mythbound`                           |
| `SERVER_IP`                 | The IP address for the server                       | `127.0.0.1`                           |
| `ASSETS_REPOSITORY`         | URL of the repository for assets                    | `https://github.com/tibia-oce/assets` |
| `REPOSITORY_BRANCH`         | Branch of the assets repository to use              | `master`                              |
