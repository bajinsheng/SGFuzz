# SFuzzer: Stateful Greybox Fuzzer
SFuzzer (Stateful Greybox Fuzzer) is a greybox fuzzer for stateful software systems built on top of [LibFuzzer](https://llvm.org/docs/LibFuzzer.html) and involves additional feedback to facilitate exploring the state space of stateful software systems for exposing stateful bugs.

# Publication

Please check more technical details on our paper: [https://arxiv.org/abs/2204](https://arxiv.org/abs/2204).


# Usage
## Running environment
Linux (Tested in Ubuntu 16.04LTS)

LLVM >= 6.0 (tested in LLVM 10)

Clang >= 6.0 (tested in Clang 10)

Python3

### 1. Build SFuzzer driver
Enter the root of the repository and run the following command to compile the SFuzzer driver.
```
./build.sh
```


### 2. State instrumentation
SFuzzer needs statical instrumentation for state feedback. It is source-code level instrumentation that is independent of any compiler.

1) State instrumentation:
```
python3 sanitizer/State_machine_instrument.py target_folder [-b blocked_variable_file]
```
* Optional step:
We also provide a debug option in the python script to output all instrumented variable names and locations.
It is better to check all instrumented variables and filter out some improper variables by [-b blocked_variable_file], such as the variables that are not enum variables, or that are too sensitive to their values.
It is not necessary but provides better results.
The reason is that our method is using regex match to extract all enum variables for state feedback. However, sometimes, some non-enum variables are extracted as enum variables because they have the exact same variable names as some enum variables. It can be solved by some compiling tools, such as clangAST, to accurately identify the enum variables. The engineering work is left for future work.


### 3. Compile the target program
Here, we follow the normal steps to compile the target program as what LibFuzzer does.
Please refer to the official [LibFuzzer] (https://llvm.org/docs/LibFuzzer.html) document for more information. Then, in the linking stage, we link the SFuzzer library to the target program "```libsfuzzer.a```".
```
clang -o a.o a.c -fsanitize=fuzzer-no-link
clang++ -o program a.o b.o c.o ... libsfuzzer.a -ldl -lpthread -fsanitize=fuzzer-no-link
```

### 4. Run the program
```
./program
```

## OpenSSL example
We provide an example to compile SFuzzer with OpenSSL. Please check the document "example/openssl/Readme.md".

# FuzzBench integration
SFuzzer aims to fuzz stateful protocol programs which usually have complicated compilation steps. We also integrate SFuzzer into FuzzBench and publicize our code to facilitate reproducibility. Please refer to this repo:.

# License
This project is licensed under the Apache License 2.0 - see the [LICENSE](./LICENSE) file for details. 
