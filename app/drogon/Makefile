
# must match the version in the Dockerfile
drogon_version=1.8.7

build:
	podman build . -t localhost/drogon:$(drogon_version)

push:
	podman push localhost/drogon:$(drogon_version) docker.io/ramosfabiano/drogon:$(drogon_version)
