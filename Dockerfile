# Build Stage
FROM --platform=linux/amd64 ubuntu:20.04 as builder

## Install build dependencies.
RUN apt-get update && \
    DEBIAN_FRONTEND=noninteractive apt-get install -y cmake

RUN apt-get update && \
    DEBIAN_FRONTEND=noninteractive apt-get install -y g++

## Add source code to the build stage.
ADD . /p7zip
WORKDIR /p7zip

## TODO: ADD YOUR BUILD INSTRUCTIONS HERE.
RUN make 7z

#Package Stage
FROM --platform=linux/amd64 ubuntu:20.04

## TODO: Change <Path in Builder Stage>
COPY --from=builder /p7zip/bin/7z /
COPY --from=builder /p7zip/bin/7z.so /
