FROM docker.io/ramosfabiano/drogon:1.8.7

WORKDIR /app

COPY app/ .

RUN cmake -Bbuild &&  make -Cbuild install

CMD ["/usr/local/bin/rinha"]
