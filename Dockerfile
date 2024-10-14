# Stage 1: Install system dependencies and vcpkg
FROM ubuntu:22.04 AS dependencies
ENV VCPKG_FORCE_SYSTEM_BINARIES=1
ENV VCPKG_ROOT=/bts/vcpkg

RUN apt update && apt install -y \
    unzip make g++ uuid-dev wget curl zip tar git \
    pkg-config cmake ninja-build python3

RUN mkdir -p /bts /usr/src/bts
RUN git clone https://github.com/microsoft/vcpkg.git /bts/vcpkg && \
    /bts/vcpkg/bootstrap-vcpkg.sh

COPY vcpkg.json /usr/src/bts/
WORKDIR /usr/src/bts
RUN /bts/vcpkg/vcpkg install


# Stage 2: Install Premake after vcpkg is ready
FROM dependencies AS premake
RUN cd /bts && \
    wget https://github.com/premake/premake-core/archive/refs/heads/master.zip && \
    unzip master.zip && rm master.zip && \
    cd premake-core-master && make -f Bootstrap.mak linux && \
    mv bin/release/premake5 /bin/ && rm -rf /bts/premake-core-master


# Stage 3: Build the project with Premake and vcpkg
FROM premake AS build
ARG RELEASE_ARCH=release_arm64
WORKDIR /usr/src/bts
COPY premake5.lua /usr/src/bts/
COPY src /usr/src/bts/src/
RUN premake5 gmake2 && make -j$(nproc) config=${RELEASE_ARCH}


# Stage 4: Create the final image
FROM ubuntu:22.04 AS final
COPY --from=build /usr/src/bts/Black-Tek-Server /app/Black-Tek-Server
COPY data /app/data
COPY *.sql key.pem /app/
COPY config.lua.dist /app/config.lua
RUN groupadd -r btsuser && \
    useradd -r -g btsuser -d /app -s /sbin/nologin btsuser && \
    chmod +x /app/Black-Tek-Server && \
    chown -R btsuser:btsuser /app
USER btsuser
WORKDIR /app
ENTRYPOINT ["./Black-Tek-Server"]
