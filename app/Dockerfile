FROM debian:bookworm-slim
#FROM ubuntu:23.10

RUN apt update

#libboost-all-dev
RUN apt install -y build-essential cmake git 

# drogon 
RUN apt install -y libjsoncpp-dev uuid-dev libssl-dev doxygen libhiredis-dev brotli libpq-dev libpqxx-dev  zlib1g-dev libsqlite3-dev 
#RUN apt install -y drogon libdrogon-dev 

WORKDIR /app

COPY app/ .

RUN cmake -Bbuild &&  make -Cbuild install -j4

CMD ["/usr/local/bin/rinha"]