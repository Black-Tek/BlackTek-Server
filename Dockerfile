FROM ubuntu:24.04 AS base

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    g++ \
    make \
    git \
    curl \
    zip \
    unzip \
    tar \
    pkg-config \
    uuid-dev \
    wget \
    ca-certificates \
    python3 \
    && rm -rf /var/lib/apt/lists/*

# install premake5
FROM base AS premake
RUN git clone --depth 1 https://github.com/premake/premake-core.git /tmp/premake && \
    cd /tmp/premake && make -f Bootstrap.mak linux && \
    mv bin/release/premake5 /usr/local/bin/premake5

# install vcpkg and C++ libraries
FROM base AS dependencies

COPY --from=premake /usr/local/bin/premake5 /usr/local/bin/premake5

RUN git clone --depth 1 https://github.com/microsoft/vcpkg.git /opt/vcpkg && \
    /opt/vcpkg/bootstrap-vcpkg.sh -disableMetrics

WORKDIR /build

COPY vcpkg.json premake5.lua ./

RUN /opt/vcpkg/vcpkg install --triplet x64-linux

# compile project
FROM dependencies AS build

COPY src/ ./src/

ARG BUILD_TYPE=release

RUN premake5 gmake2 && make -j$(nproc) config=${BUILD_TYPE}_64

FROM ubuntu:24.04

RUN apt-get update && apt-get install -y --no-install-recommends \
    libstdc++6 \
    libgcc-s1 \
    && rm -rf /var/lib/apt/lists/*

COPY --from=build /build/Black-Tek-Server /bin/bts
COPY data /srv/data/
COPY README.md *.dist *.sql key.pem /srv/

EXPOSE 7171 7172
WORKDIR /srv
VOLUME /srv
ENTRYPOINT ["/bin/bts"]