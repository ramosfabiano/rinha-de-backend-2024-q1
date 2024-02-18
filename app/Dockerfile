FROM docker.io/ramosfabiano/drogon:official-20240218

WORKDIR /app

COPY . .

RUN cmake -Bbuild &&  make -Cbuild install

COPY config.json /etc/rinha/config.json

CMD ["/usr/local/bin/rinha"]
