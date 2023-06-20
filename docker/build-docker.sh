#!/usr/bin/env bash

export LC_ALL=C

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd $DIR/.. || exit

DOCKER_IMAGE=${DOCKER_IMAGE:-The-Neoxa-Endeavor/neoxad-develop}
DOCKER_TAG=${DOCKER_TAG:-latest}

BUILD_DIR=${BUILD_DIR:-.}

rm docker/bin/*
mkdir docker/bin
cp $BUILD_DIR/src/neoxad docker/bin/
cp $BUILD_DIR/src/neoxa-cli docker/bin/
cp $BUILD_DIR/src/neoxa-tx docker/bin/
strip docker/bin/neoxad
strip docker/bin/neoxa-cli
strip docker/bin/neoxa-tx

docker build --pull -t $DOCKER_IMAGE:$DOCKER_TAG -f docker/Dockerfile docker
