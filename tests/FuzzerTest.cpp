#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

__attribute__((weak)) extern "C" void __sfuzzer_instrument(uint32_t state_name, uint32_t state_value);

bool GlobalFlag = false;

int initialize() {
  GlobalFlag = false;
  return 1;
}

bool level3_1(const uint8_t *Data) {  
    __sfuzzer_instrument(1,1);
  return true;
}

bool level2_1(const uint8_t *Data) {
    __sfuzzer_instrument(2,1);
  if (Data[0] == 'D') {
    return level3_1(Data+1);
  }
  return false;
}

bool level2_2(const uint8_t *Data) {
    __sfuzzer_instrument(3,1);
  return true;
}

bool level2_3(const uint8_t *Data) {
    __sfuzzer_instrument(4,1);
  return true;
}

bool level2_4(const uint8_t *Data) {
    __sfuzzer_instrument(5,1);
  return true;
}

bool level1_1(const uint8_t *Data) {
    __sfuzzer_instrument(6,1);
  if (Data[0] == 'C') {
    return level2_1(Data+1);
  } else {
    return level2_2(Data+1);
  }
}

bool level1_2(const uint8_t *Data) {
  if (Data[0] == 'E') {
    return level2_3(Data+1);
  } else {
    return level2_4(Data+1);
  }

}

bool level1_3(const uint8_t *Data) {
  return true;
}

bool FuzzMe(const uint8_t *Data, size_t DataSize) {
  if (Data[0] == 'A') {
    GlobalFlag = true;
    return level1_1(Data+1);
  } else if (Data[0] == 'B') {
    if (GlobalFlag) {
      return level1_2(Data + 1);
    }
    return level1_3(Data + 1);
  } else {
    return level1_3(Data + 1);
  }
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  FuzzMe(Data, Size);
  return 0;
}