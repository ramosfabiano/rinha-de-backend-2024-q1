#!/bin/bash

TAG="v2"

podman-compose build --no-cache
if [ $? -ne 0 ]; then
  echo "Build failed"
  exit -1
fi

podman login docker.io -u ramosfabiano -p $DOCKER_PASSWORD

podman push localhost/rinha-de-backend-2024-q1_postgres01 ramosfabiano/rinha-de-backend-2024-q1_postgres01:$TAG
podman push localhost/rinha-de-backend-2024-q1_postgres02 ramosfabiano/rinha-de-backend-2024-q1_postgres02:$TAG
podman push localhost/rinha-de-backend-2024-q1_nginx ramosfabiano/rinha-de-backend-2024-q1_nginx:$TAG
podman push localhost/rinha-de-backend-2024-q1_api01 ramosfabiano/rinha-de-backend-2024-q1_api:$TAG
