FROM ubuntu:16.04

# Setup ZMQ, cd /tmp/libzmq && make check
RUN apt-get update && \
    DEBIAN_FRONTEND=noninteractive apt-get install -y git build-essential libtool autoconf automake pkg-config unzip libkrb5-dev && \
    cd /tmp && git clone https://github.com/jedisct1/libsodium.git && cd libsodium && git checkout e2a30a && ./autogen.sh && ./configure && make check && make install && ldconfig && \
    cd /tmp && git clone --depth 1 --branch v4.2.1 https://github.com/zeromq/libzmq.git && cd libzmq && ./autogen.sh && ./configure && make && \
    cd /tmp/libzmq && make install && ldconfig && \
    rm /tmp/* -rf

# Setup CMake
RUN apt-get install wget -y && \
    wget https://cmake.org/files/v3.11/cmake-3.11.3.tar.gz -P /tmp/ && \
    tar xzf /tmp/cmake-3.11.3.tar.gz -C /tmp/ && \
    cd /tmp/cmake-3.11.3 && ./configure --prefix=/opt/cmake && make -j4 && make install && \
    /opt/cmake/bin/cmake -version && \
    rm /tmp/* -rf

RUN mkdir -p /opt/src
WORKDIR /opt/src
COPY . /opt/src

EXPOSE 4040

CMD ./actions.sh