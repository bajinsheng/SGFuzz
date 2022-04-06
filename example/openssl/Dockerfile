FROM ubuntu:16.04

ENV HFND_TCP_PORT=4433

RUN chmod 777 /usr/lib &&\
    chmod 777 /opt

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
    libgnutls-dev \
    python-pip \
    nano \
    python-jinja2 \
    net-tools \
    vim \
    clang-10 \
    gdb \
    psmisc \
    screen

# Add a new user ubuntu, pass: ubuntu
RUN groupadd ubuntu && \
    useradd -rm -d /home/ubuntu -s /bin/bash -g ubuntu -G sudo -u 1000 ubuntu -p "$(openssl passwd -1 ubuntu)"

# Use ubuntu as default username
USER ubuntu
WORKDIR /home/ubuntu


RUN git clone https://github.com/google/honggfuzz.git && \
    cd honggfuzz && \
    git checkout 6f89ccc9c43c6c1d9f938c81a47b72cd5ada61ba && \
    CC=clang-10 CFLAGS="-fsanitize=fuzzer-no-link -fsanitize=address" make libhfcommon/libhfcommon.a && \
    CC=clang-10 CFLAGS="-fsanitize=fuzzer-no-link -fsanitize=address -DHFND_RECVTIME=1" make libhfnetdriver/libhfnetdriver.a && \
    mv libhfcommon/libhfcommon.a /usr/lib/libhfcommon.a && \
    mv libhfnetdriver/libhfnetdriver.a /usr/lib/libhfnetdriver.a

# Set up SFuzzer
COPY --chown=ubuntu:ubuntu SFuzzer SFuzzer
RUN cd SFuzzer && \
    ./build.sh && \
    cp libsfuzzer.a /usr/lib/libsFuzzer.a

# Set up environment variables for AFLNet
ENV WORKDIR="/home/ubuntu/experiments"
ENV PATH="${PATH}:/home/ubuntu/.local/bin:${WORKDIR}"


RUN mkdir $WORKDIR 

COPY --chown=ubuntu:ubuntu key.pem ${WORKDIR}/key.pem
COPY --chown=ubuntu:ubuntu cert.pem ${WORKDIR}/cert.pem
COPY --chown=ubuntu:ubuntu in-tls ${WORKDIR}/in-tls
COPY --chown=ubuntu:ubuntu tls.dict ${WORKDIR}/tls.dict
COPY --chown=ubuntu:ubuntu blocked_variables.txt ${WORKDIR}/blocked_variables.txt


RUN cd ${WORKDIR} && \
    git clone https://github.com/openssl/openssl.git openssl-sfuzzer && \
    cd openssl-sfuzzer && \
    cp ${WORKDIR}/*.pem ./ && \
    git checkout c74188e && \
    sed -i "s/ main/ HonggfuzzNetDriver_main/g" apps/openssl.c && \
    python3 /home/ubuntu/SFuzzer/sanitizer/State_machine_instrument.py ${WORKDIR}/openssl-sfuzzer/ -b ${WORKDIR}/blocked_variables.txt && \
    CC=clang-10 CXX=clang++-10 CFLAGS="-fsanitize=fuzzer-no-link -fsanitize=address" ./config no-shared && \
    make build_generated && \
    make apps/openssl || clang++-10 -fsanitize=fuzzer-no-link -fsanitize=address -lsFuzzer -lhfnetdriver -lhfcommon \
        -pthread -m64 -Wa,--noexecstack -Qunused-arguments -Wall -O3 -L.   \
        -o apps/openssl \
        apps/lib/openssl-bin-cmp_mock_srv.o \
        apps/openssl-bin-asn1parse.o apps/openssl-bin-ca.o \
        apps/openssl-bin-ciphers.o apps/openssl-bin-cmp.o \
        apps/openssl-bin-cms.o apps/openssl-bin-crl.o \
        apps/openssl-bin-crl2pkcs7.o apps/openssl-bin-dgst.o \
        apps/openssl-bin-dhparam.o apps/openssl-bin-dsa.o \
        apps/openssl-bin-dsaparam.o apps/openssl-bin-ec.o \
        apps/openssl-bin-ecparam.o apps/openssl-bin-enc.o \
        apps/openssl-bin-engine.o apps/openssl-bin-errstr.o \
        apps/openssl-bin-fipsinstall.o apps/openssl-bin-gendsa.o \
        apps/openssl-bin-genpkey.o apps/openssl-bin-genrsa.o \
        apps/openssl-bin-info.o apps/openssl-bin-kdf.o \
        apps/openssl-bin-list.o apps/openssl-bin-mac.o \
        apps/openssl-bin-nseq.o apps/openssl-bin-ocsp.o \
        apps/openssl-bin-openssl.o apps/openssl-bin-passwd.o \
        apps/openssl-bin-pkcs12.o apps/openssl-bin-pkcs7.o \
        apps/openssl-bin-pkcs8.o apps/openssl-bin-pkey.o \
        apps/openssl-bin-pkeyparam.o apps/openssl-bin-pkeyutl.o \
        apps/openssl-bin-prime.o apps/openssl-bin-progs.o \
        apps/openssl-bin-rand.o apps/openssl-bin-rehash.o \
        apps/openssl-bin-req.o apps/openssl-bin-rsa.o \
        apps/openssl-bin-rsautl.o apps/openssl-bin-s_client.o \
        apps/openssl-bin-s_server.o apps/openssl-bin-s_time.o \
        apps/openssl-bin-sess_id.o apps/openssl-bin-smime.o \
        apps/openssl-bin-speed.o apps/openssl-bin-spkac.o \
        apps/openssl-bin-srp.o apps/openssl-bin-storeutl.o \
        apps/openssl-bin-ts.o apps/openssl-bin-verify.o \
        apps/openssl-bin-version.o apps/openssl-bin-x509.o \
        apps/libapps.a -lssl -lcrypto -ldl -pthread


# Run command
# ./apps/openssl -close_fd_mask=3 ../in-tls -- s_server -key ../key.pem -cert ../cert.pem -4 -no_anti_replay 
