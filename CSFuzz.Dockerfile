FROM ubuntu:focal

RUN apt-get update -y && apt-get install -y \
    git \
    clang \
    llvm-dev \
    libc++-dev \
    libc++abi-dev

RUN git clone https://github.com/csfuzz/CSFuzz.git

WORKDIR /CSFuzz