#!/bin/bash

TAG="v1"

podman compose build --no-cache
if $? -ne 0; then
  echo "Build failed"
  exit -1
fi

podman login docker.io -u ramosfabiano -p $DOCKER_PASSWORD

IMAGE_LIST=$(podman image list | grep "localhost/rinha-de-backend-2024-q1" | awk '{print $1":"$2}' | cut -d '_' -f 2 | cut -d ':' -f 1)

for i in $IMAGE_LIST; do
  podman push localhost/rinha-de-backend-2024-q1_$i ramosfabiano/rinha-de-backend-2024-q1_$i:$TAG
done
