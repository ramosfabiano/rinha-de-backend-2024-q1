FROM docker.io/ramosfabiano/drogon:official-20240218

WORKDIR /app

COPY . .

RUN cmake -Bbuild &&  make -Cbuild install

COPY config.json /etc/rinha/config.json

#CMD ["/usr/local/bin/rinha"]
CMD ["/bin/bash", "-c", "/usr/local/bin/rinha > /dev/null 2>&1"]
