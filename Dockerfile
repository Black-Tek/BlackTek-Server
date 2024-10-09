# Stage 1: Dependencies
FROM ubuntu:22.04 AS dependencies
ENV VCPKG_FORCE_SYSTEM_BINARIES=1
ENV VCPKG_ROOT=/bts/vcpkg

RUN apt update && apt install -y \
    unzip \
    make \
    g++ \
    uuid-dev \
    wget \
    curl \
    zip \
    tar \
    git \
    pkg-config \
    cmake \
    ninja-build \
    python3 \
    gcc-aarch64-linux-gnu \
    g++-aarch64-linux-gnu

RUN mkdir /bts && \
    cd /bts && \
    git clone --depth 1 https://github.com/premake/premake-core.git && \
    cd premake-core && \
    make -f Bootstrap.mak linux && \
    mv bin/release/premake5 /bin/premake5 && \
    rm -rf /bts/premake-core

RUN git clone https://github.com/microsoft/vcpkg.git /bts/vcpkg && \
    /bts/vcpkg/bootstrap-vcpkg.sh


# Stage 2: Build
FROM dependencies AS build
ENV VCPKG_FORCE_SYSTEM_BINARIES=1
ENV VCPKG_ROOT=/bts/vcpkg

COPY src /usr/src/bts/src/
COPY premake5.lua vcpkg.json /usr/src/bts/
WORKDIR /usr/src/bts
RUN /bts/vcpkg/vcpkg install --triplet arm64-linux
RUN premake5 gmake2 
RUN make -j 2 config=release_arm64


# Stage 3: Final runtime image
FROM ubuntu:22.04 AS final
COPY --from=build /usr/src/bts/Black-Tek-Server /app/Black-Tek-Server
COPY data /app/data
COPY *.sql key.pem /app/
COPY config.lua.dist /app/config.lua
RUN groupadd -r btsuser && useradd -r -g btsuser -d /app -s /sbin/nologin btsuser
RUN chmod +x /app/Black-Tek-Server && chown -R btsuser:btsuser /app
USER btsuser
WORKDIR /app
ENTRYPOINT ["./Black-Tek-Server"]
