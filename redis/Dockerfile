FROM docker.io/redis:7.2.4

COPY redis.conf /etc/redis-custom.conf 
RUN chown redis.redis  /etc/redis-custom.conf 

RUN echo "vm.overcommit_memory = 1" >> /etc/sysctl.conf

EXPOSE 6379

CMD ["redis-server", "/etc/redis-custom.conf"]

