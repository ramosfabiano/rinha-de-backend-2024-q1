FROM docker.io/postgres:15.6

COPY postgres/config.sql /docker-entrypoint-initdb.d/config.sql
COPY postgres/create-table.sql /docker-entrypoint-initdb.d/create-table.sql
RUN chown -R postgres.postgres /docker-entrypoint-initdb.d/

