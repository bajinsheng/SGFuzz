# SGFuzz: Stateful Greybox Fuzzer
SGFuzz (Stateful Greybox Fuzzer) is a greybox fuzzer for stateful software systems built on top of [LibFuzzer](https://llvm.org/docs/LibFuzzer.html) and involves additional feedback to facilitate exploring the state space of stateful software systems for exposing stateful bugs.

# Publication

Please check more technical details on our paper published at Usenix Security 2022: [https://www.usenix.org/conference/usenixsecurity22/presentation/ba](https://www.usenix.org/conference/usenixsecurity22/presentation/ba).

# How to run?
We provide a docker file to execute SGFuzz with OpenSSL. Please refer to the document [example/openssl/Readme.md](https://github.com/bajinsheng/SGFuzz/tree/master/example/openssl). This example shows how to fuzz a stateful protocol program without customized fuzzing harness.

# Detailed Usage
## Running Environment
Linux (Tested in Ubuntu 20.04LTS)

LLVM >= 6.0 (tested in LLVM 10)

Clang >= 6.0 (tested in Clang 10)

Python3.8

### 1. Build SGFuzz Driver
Enter the root of the repository and run the following command to compile the SGFuzz driver.
```
./build.sh
```


### 2. State Instrumentation
SGFuzz needs statical instrumentation for state feedback. It is source-code level instrumentation that is independent of any compiler.

1) State instrumentation:
```
python3 sanitizer/State_machine_instrument.py target_folder [-b blocked_variable_file]
```
* Optional step:
We also provide a debug option in the python script to output all instrumented variable names and locations.
It is better to check all instrumented variables and filter out some improper variables by [-b blocked_variable_file], such as the variables that are not enum variables, or that are too sensitive to their values.
It is not necessary but provides better results.
The reason is that our method is using regex match to extract all enum variables for state feedback. However, sometimes, some non-enum variables are extracted as enum variables because they have the exact same variable names as some enum variables. It can be solved by some compiling tools, such as clangAST, to accurately identify the enum variables. The engineering work is left for future work.


### 3. Compilation
Here, we follow the normal steps to compile the target program as what LibFuzzer does.
Please refer to the official [LibFuzzer](https://llvm.org/docs/LibFuzzer.html) document for more information. Then, in the linking stage, we link the SGFuzz library to the target program "```libsfuzzer.a```".
```
clang -o a.o a.c -fsanitize=fuzzer-no-link
clang++ -o program a.o b.o c.o ... libsfuzzer.a -ldl -lpthread -fsanitize=fuzzer-no-link
```

### 4. Running
```
./program
```

# License
This project is licensed under the Apache License 2.0 - see the [LICENSE](./LICENSE) file for details. 
