#!/bin/sh

# 1. Build sfuzzer driver
echo "Building sfuzzer driver..."
LIBFUZZER_SRC_DIR=$(dirname $0)
for f in $LIBFUZZER_SRC_DIR/*.cpp; do
  clang++ -g -fPIC -O2 -std=c++11 $f -c &
done
wait
rm -f libsfuzzer.a
ar r libsfuzzer.a Fuzzer*.o
rm -f Fuzzer*.o

