FROM ubuntu:16.04

ENV HFND_TCP_PORT=8554

RUN chmod 777 /usr/lib

# Enable LLVM-10.0 in ubuntu 16.04
RUN apt-get update \
 && apt-get install -y --no-install-recommends curl ca-certificates gnupg2 apt-transport-https software-properties-common \
 && echo 'deb http://apt.llvm.org/xenial/ llvm-toolchain-xenial-10 main' >> /etc/apt/sources.list.d/llvm10.list \
 && echo 'deb-src http://apt.llvm.org/xenial/ llvm-toolchain-xenial-10 main' >> /etc/apt/sources.list.d/llvm10.list \
 && curl https://apt.llvm.org/llvm-snapshot.gpg.key | apt-key add -

# Install common dependencies
RUN apt-get -y update && \
    apt-get -y install sudo \ 
    apt-utils \
    build-essential \
    openssl \
    clang \
    graphviz-dev \
    git \
    wget \
    libgnutls-dev \
    python-pip \
    nano \
    net-tools \
    vim \
    python-jinja2 \
    clang-10 \
    gdb \
    psmisc

# Add a new user ubuntu, pass: ubuntu
RUN groupadd ubuntu && \
    useradd -rm -d /home/ubuntu -s /bin/bash -g ubuntu -G sudo -u 1000 ubuntu -p "$(openssl passwd -1 ubuntu)"

# Use ubuntu as default username
USER ubuntu
WORKDIR /home/ubuntu

RUN git clone https://github.com/google/honggfuzz.git && \
    cd honggfuzz && \
    git checkout 6f89ccc9c43c6c1d9f938c81a47b72cd5ada61ba && \
    #sed -i "s/-D_HF_ARCH_\${ARCH}//g" Makefile && \
    #sed -i "s/INFO;/ERROR;/g" libhfcommon/log.c && \
    CC=clang-10 CFLAGS="-g -fsanitize=fuzzer-no-link -fsanitize=address" make libhfcommon/libhfcommon.a && \
    CC=clang-10 CFLAGS="-g -fsanitize=fuzzer-no-link -fsanitize=address -DHFND_RECVTIME=1" make libhfnetdriver/libhfnetdriver.a && \
    mv libhfcommon/libhfcommon.a /usr/lib/libhfcommon.a && \
    mv libhfnetdriver/libhfnetdriver.a /usr/lib/libhfnetdriver.a

# Set up SGFuzz
RUN git clone https://github.com/bajinsheng/SGFuzz.git && \
        cd SGFuzz && \
        ./build.sh && \
        cp libsfuzzer.a /usr/lib/libsFuzzer.a

ENV WORKDIR="/home/ubuntu/experiments"

COPY --chown=ubuntu:ubuntu fuzzing.patch ${WORKDIR}/fuzzing.patch
COPY --chown=ubuntu:ubuntu in-rtsp ${WORKDIR}/in-rtsp
COPY --chown=ubuntu:ubuntu rtsp.dict ${WORKDIR}/rtsp.dict
COPY --chown=ubuntu:ubuntu live.2021.08.13.tar.gz ${WORKDIR}/
COPY --chown=ubuntu:ubuntu sample_media_sources ${WORKDIR}/sample_media_sources
COPY --chown=ubuntu:ubuntu blocked_variables.txt ${WORKDIR}/blocked_variables.txt

# Download Live555
RUN cd $WORKDIR && \
    tar -zxvf live.2021.08.13.tar.gz && \
    chmod 777 -R live

# Download and compile Live555 for SGFuzz
RUN cd $WORKDIR && \
    cp -r live live555-sgfuzz && \
    cd live555-sgfuzz && \
    patch -p1 < $WORKDIR/fuzzing.patch && \
    sed -i "s/int main(/extern \"C\" int HonggfuzzNetDriver_main(/g" testProgs/testOnDemandRTSPServer.cpp && \
    python3 /home/ubuntu/SGFuzz/sanitizer/State_machine_instrument.py ${WORKDIR}/live555-sgfuzz/ -b ${WORKDIR}/blocked_variables.txt && \
    ./genMakefiles linux-no-openssl && \
    make C_COMPILER=clang-10 CPLUSPLUS_COMPILER=clang++-10 CFLAGS="-g -fsanitize=fuzzer-no-link -fsanitize=address" CXXFLAGS="-g -fsanitize=fuzzer-no-link -fsanitize=address" LINK="clang++-10 -fsanitize=fuzzer-no-link -fsanitize=address -o " all || cd testProgs; clang++-10 -fsanitize=fuzzer-no-link -fsanitize=address -o testOnDemandRTSPServer -L.  testOnDemandRTSPServer.o announceURL.o ../liveMedia/libliveMedia.a ../groupsock/libgroupsock.a ../BasicUsageEnvironment/libBasicUsageEnvironment.a ../UsageEnvironment/libUsageEnvironment.a -lsFuzzer -lhfnetdriver -lhfcommon


# Set up Live555
RUN cp ${WORKDIR}/sample_media_sources/*.* ${WORKDIR}/live555-sgfuzz/testProgs/
