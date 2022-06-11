# SGFuzz: Stateful Greybox Fuzzer
SGFuzz (Stateful Greybox Fuzzer) is a greybox fuzzer for stateful software systems built on top of [LibFuzzer](https://llvm.org/docs/LibFuzzer.html) and involves additional feedback to facilitate exploring the state space of stateful software systems for exposing stateful bugs.

# Publication

Please check more technical details on our paper: [http://arxiv.org/abs/2204.02545](http://arxiv.org/abs/2204.02545).

# How to run?
We provide a docker file to execute SGFuzz with OpenSSL. Please refer to the document [example/openssl/Readme.md](https://github.com/bajinsheng/SGFuzz/tree/master/example/openssl). This example shows how to fuzz a stateful protocol program without customized fuzzing harness.

# Artifact Evaluation
## Our claims and results
1. **State Transition Coverage**. SGFuzz covers 33x more sequences of state transitions than LibFuzzer in 23 hours on average. (RQ.1)
2. **Branch Coverage**. SGFuzz achieves 2.20\% more branch coverage than LibFuzzer in 23 hours on average. (RQ.2)
3. **State Identification Effectiveness**. Average 99.5% of nodes in the STT constructed in 23 hours are referring to values of actual state variables. (RQ.3)
4. **Prevalence of Stateful Bugs**. Every four in five bugs that are reported in OSS-Fuzz for protocol implementations among our subjects are stateful. (Appendix.3)
5. **Prevalence of State Variables**. Top-50 most widely used open-source protocol implementations define state variables with named constants. (Appendix.4)

## Prerequistities
We have integrated our code into the FuzzBench framework, so the dependencies of FuzzBench are enough to evaluate our code. 
Please refer to the following commands to install and configure the FuzzBench.
```shell
git clone https://github.com/bajinsheng/SGFuzz_Fuzzbench
cd SGFuzz_Fuzzbench
git submodule update --init
sudo apt-get install build-essential python3.8-dev python3.8-venv
make install-dependencies
source .venv/bin/activate
```
More information about the installation of Fuzzbench can be found: [https://google.github.io/fuzzbench/getting-started/prerequisites/](https://google.github.io/fuzzbench/getting-started/prerequisites/).

Note that the FuzzBench framework depends on docker, so it is hard to run FuzzBench within docker.

## Steps to reproduce
1. **State Transition Coverage**. 

- Step 1: Executing this command in the root of SGFuzz_FuzzBench folder: `sudo make run-sfuzzer-h2o_h2o-fuzzer-http2`

- Step 2: After prompting some building information (several minutes for the first time), the fuzzing status will be gradually output in the terminal, like this:
```shell
#2      INITED cov: 641 ft: 642 corp: 1/12569b exec/s: 0 rss: 38Mb states: 13 leaves: 2
#3      NEW    cov: 649 ft: 659 corp: 2/24Kb lim: 12569 exec/s: 0 rss: 39Mb states: 13 leaves: 2 L: 12569/12569 MS: 1 CopyPart-
```
The number of *leaves* represents the number of unique state transition sequences observed in the current fuzzing campaign.

- Step 3: Run `sudo make run-libfuzzer-h2o_h2o-fuzzer-http2` again to get the result of LibFuzzer.

- Step 4: Compare the number of *leaves* indicated in each fuzzing campaign. Note that our experiments were conducted in 23 hours, so we may notice a substantial gap in state transition coverage between SGFuzz and LibFuzzer after **several hours**, not a few minutes.

- Step 5: Changing `h2o_h2o-fuzzer-http2` to `curl_curl_fuzzer`, `mbedtls_fuzz_dtlsserver`, `gstreamer_gst-discoverer` in the command and redo steps 1-4 to evaluate other subjects. Note that the results may have a big fluctuation (Up to 50%) because of the randomness of fuzzing.

2. **Branch Coverage**. 
The same steps as the state transition coverage experiment. The branch coverage information is indicated as number of *cov* in the output. 
Note that the results may have a fluctuation (Up to 20%) because of the randomness of fuzzing.

3. **State Identification Effectiveness**. 
Please check the folder *RQ3_State_Iden_Effic* at [https://zenodo.org/record/5944816](https://zenodo.org/record/5944816), which includes all state variables and the variables that are included in the STT.
4. **Prevalence of Stateful Bugs**. 
Please check the folder *A3_Bug_Preva* at [https://zenodo.org/record/5944816](https://zenodo.org/record/5944816), which includes our analysis of existing bugs.
5. **Prevalence of State Variables**. 
Please check the folder *A4_Top50* at [https://zenodo.org/record/5944816](https://zenodo.org/record/5944816), which includes state variable names and corresponding code locations.


# Code Reference
Our major implementation of the STT is here [FuzzerStateMachine.cpp](https://github.com/bajinsheng/SGFuzz/blob/master/FuzzerStateMachine.cpp)

# Detailed Usage
## Running Environment
Linux (Tested in Ubuntu 16.04LTS)

LLVM >= 6.0 (tested in LLVM 10)

Clang >= 6.0 (tested in Clang 10)

Python3

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
