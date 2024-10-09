# Stage 1: Dependencies
FROM ubuntu:22.04 AS dependencies

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
    cmake

RUN mkdir /bts && \
    cd /bts && \
    wget https://github.com/premake/premake-core/archive/refs/heads/master.zip && \
    unzip master.zip && \
    cd premake-core-master && \
    make -f Bootstrap.mak linux && \
    mv bin/release/premake5 /bin/premake5 && \
    rm -rf /bts/premake-core-master master.zip

RUN git clone https://github.com/microsoft/vcpkg.git /bts/vcpkg && \
    /bts/vcpkg/bootstrap-vcpkg.sh

# Stage 2: Build
FROM dependencies AS build

COPY src /usr/src/bts/src/
COPY premake5.lua vcpkg.json /usr/src/bts/
WORKDIR /usr/src/bts
RUN /bts/vcpkg/vcpkg install && \
    premake5 gmake2 && \
    make -j$(nproc) config=debug_64

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
