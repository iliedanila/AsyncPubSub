# GCC support can be specified at major, minor, or micro version
# (e.g. 8, 8.2 or 8.2.0).
# See https://hub.docker.com/r/library/gcc/ for all supported GCC
# tags from Docker Hub.
# See https://docs.docker.com/samples/library/gcc/ for more on how to use this image
FROM ubuntu:latest
RUN apt update && apt install -y gcc g++ cmake libboost-all-dev

# These commands copy your files into the specified directory in the image
# and set that as the working location
COPY . /usr/src/asyncpubsub
WORKDIR /usr/src/asyncpubsub

# This command compiles your app using GCC, adjust for your source code
RUN cmake -H. -Bbuild -DCMAKE_BUILD_TYPE=Release
RUN cmake --build build/

# This command runs your application, comment out this line to compile only
# CMD ["./build/Playground/playground"]

LABEL Name=asyncpubsub Version=0.0.1
