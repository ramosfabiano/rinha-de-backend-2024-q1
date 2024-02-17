FROM docker.io/debian:bookworm-slim

RUN apt update
RUN apt install -y build-essential cmake git 
RUN apt install -y libjsoncpp-dev uuid-dev libssl-dev doxygen libhiredis-dev brotli libpq-dev libpqxx-dev zlib1g-dev libsqlite3-dev libmariadb-dev

WORKDIR /drogon-build

RUN git clone --branch v1.8.7 https://github.com/drogonframework/drogon && \ 
    cd drogon && \
    git submodule update --init && \
    ./build.sh

WORKDIR /
RUN rm -rf /drogon-build