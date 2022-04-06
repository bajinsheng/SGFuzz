## A detailed tutorial to build SFuzzer with OpenSSL

### 1. Build and install SFuzzer
 ```shell
git clone https://github.com/bajinsheng/SFuzzer
cd SFuzzer
./build.sh 
cp libsfuzzer.a /usr/lib/libsFuzzer.a
 ```

### 2. Build and install Honggfuzz
Since we use the netdriver of Honggfuzz, we also need to build and install the netdriver component from Honggfuzz.
 ```shell
git clone https://github.com/google/honggfuzz.git && \
    cd honggfuzz && \
    git checkout 6f89ccc9c43c6c1d9f938c81a47b72cd5ada61ba && \
    CC=clang-10 CFLAGS="-fsanitize=fuzzer-no-link -fsanitize=address" make libhfcommon/libhfcommon.a && \
    CC=clang-10 CFLAGS="-fsanitize=fuzzer-no-link -fsanitize=address -DHFND_RECVTIME=1" make libhfnetdriver/libhfnetdriver.a && \
    mv libhfcommon/libhfcommon.a /usr/lib/libhfcommon.a && \
    mv libhfnetdriver/libhfnetdriver.a /usr/lib/libhfnetdriver.a
 ```

### 3. Specify the tcp port used by OpenSSL
We need to tell netdriver the tcp port used by the OpenSSL
```shell
export HFND_TCP_PORT=4433
```

### 4. Download OpenSSL
```shell
git clone https://github.com/openssl/openssl.git openssl-sfuzzer && \
    cd openssl-sfuzzer && \
    git checkout c74188e
```
### 5. Instrument the state variables
```
cd ../ && \
python3 {PATH}/SFuzzer/sanitizer/State_machine_instrument.py {PATH}/openssl-sfuzzer/ -b blocked_variables.txt
```
The `-b` option is optional. It is used to block some variables which may not enum variables and may incur compilation fail or other issues. The list needs to be manually specified.

### 6. Adjust the main function for netdriver
```
cd openssl-sfuzzer && \
sed -i "s/ main/ HonggfuzzNetDriver_main/g" apps/openssl.c
```
This step is necessary to adopt netdriver.

### 7. Compile the OpenSSL
```
CC=clang-10 CXX=clang++-10 CFLAGS="-fsanitize=fuzzer-no-link -fsanitize=address" ./config no-shared && \
    make build_generated && \
    make apps/openssl
```

### 8. Link SFuzzer
Because we modifed the main function, the compilation will fail at the final link stage.
We need to rerun this step to link SFuzzer with three new parameters: "-lsFuzzer -lhfnetdriver -lhfcommon"
```
clang++-10 -fsanitize=fuzzer-no-link -fsanitize=address -lsFuzzer -lhfnetdriver -lhfcommon \
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
```
### 9. Run the target 
```
cd experiment/openssl-sfuzzer && \
./apps/openssl -close_fd_mask=3 ../in-tls -- s_server -key ../key.pem -cert ../cert.pem -4 -no_anti_replay 
```


### 10.Dockerfile
We provide a dockerfile to run all the above commands.
1) Netdriver needs privilege in docker.
2) Note that SFuzzer git repo is private, so we need to clone the SFuzzer repo to the same folder as the Dockerfile before building the docker image.
```
 docker build -t openssl .
 docker run -it --privileged openssl /bin/bash
```
