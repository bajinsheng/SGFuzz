## An example for Live555

```shell
sudo docker build . -t live555
sudo docker run -it --privileged live555 /bin/bash
cd experiments/live555-sgfuzz/testProgs/
ASAN_OPTIONS=alloc_dealloc_mismatch=0 ./testOnDemandRTSPServer -close_fd_mask=3 -detect_leaks=0 -dict=${WORKDIR}/rtsp.dict -only_ascii=1 ${WORKDIR}/in-rtsp/
```