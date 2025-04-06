FROM ubuntu:22.04

ENV TZ=Asia/Shanghai
ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && \
    apt-get install -y g++ cmake make \
    libmysqlclient-dev libhiredis-dev redis-server mysql-server \
    libcurl4-openssl-dev \
    libboost-dev libboost-test-dev libssl-dev \
    git wget unzip \
    openssh-client

#源码安装muduo
RUN git clone https://github.com/chenshuo/muduo.git /temp/muduo && \
    cd /temp/muduo && \
    mkdir build && cd build && cmake .. -DCMAKE_BUILD_TYPE=Release && \
    make -j$(nproc) && make install && ldconfig && rm -rf /temp/muduo

WORKDIR /app

COPY include /app/include
COPY src/client /app/src/client
COPY src/server /app/src/server
COPY thirdparty /app/thirdparty
COPY CMakeLists.txt /app/CMakeLists.txt

RUN mkdir build && cd build && cmake .. && make -j$(nproc)

CMD ["/bin/bash"]