FROM ubuntu:22.04 AS build

RUN apt update

# install premake5
RUN apt install -y unzip make g++ uuid-dev wget && \
    mkdir /bts && \
    cd /bts/ && wget https://github.com/premake/premake-core/archive/refs/heads/master.zip && \
    cd /bts/ && unzip master.zip && rm master.zip && \
    cd /bts/premake-core-master && make -f Bootstrap.mak linux && \
    mv /bts/premake-core-master/bin/release/premake5 /bin/premake5 && \
    rm -rf /bts/premake-core-master/bin/release/premake5

# install vcpkg
RUN apt install -y curl zip unzip tar git && \
    cd /bts && git clone https://github.com/microsoft/vcpkg.git && \
    /bts/vcpkg/bootstrap-vcpkg.sh

COPY src /usr/src/bts/src/
COPY premake5.lua vcpkg.json /usr/src/bts/

WORKDIR /usr/src/bts

# install C++ libraries from vcpkg
RUN apt install -y pkg-config
RUN cd /usr/src/bts && /bts/vcpkg/vcpkg install

# compile project
RUN premake5 gmake2 && make -j $(nproc) config=debug_64

FROM ubuntu:22.04

COPY --from=build /usr/src/bts/Black-Tek-Server /bin/bts
COPY data /srv/data/
COPY LICENSE README.md *.dist *.sql key.pem /srv/

EXPOSE 7171 7172
WORKDIR /srv
VOLUME /srv
ENTRYPOINT ["/bin/bts"]
