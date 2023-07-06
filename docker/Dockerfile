FROM debian:stretch
LABEL maintainer="Neoxa Developers <admin@neoxa.net>"
LABEL description="Dockerised NeoxaCore, built from Travis"

RUN apt-get update && apt-get -y upgrade && apt-get clean && rm -fr /var/cache/apt/*

COPY bin/* /usr/bin/
