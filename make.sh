#! /bin/bash

# Project directory (relative, without a trailing slash).
PROJECT_DIR="src"
OUTPUT_DIR="out"
# Compile command to be run in the container.
MAKE_CMD="make"

CYAN="\033[1;36m"
NC="\033[0m"

echolog ()
{
    echo -e "\n${CYAN}[INFO]: ${1}${NC}\n"
}

echolog "Creating and starting container..."
CONTAINER_ID=`docker run -v "./${PROJECT_DIR}":"/${PROJECT_DIR}" -v "./${OUTPUT_DIR}":"/${OUTPUT_DIR}" -itd docker.io/miyoocfw/toolchain-shared-uclibc:latest`

echolog "Compiling in container..."
docker exec -it -w "/${PROJECT_DIR}/" "${CONTAINER_ID}" ${MAKE_CMD}

echolog "Stopping container..."
docker stop "${CONTAINER_ID}"
echolog "Deleting container..."
docker rm "${CONTAINER_ID}"
