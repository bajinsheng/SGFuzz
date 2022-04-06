//clang++ -g -fPIC -O0 -std=c++11 ../FuzzerStateMachine.cpp StateTest.cpp -o StateTest
#include "../FuzzerStateMachine.h"
#include <iostream>
__attribute__((weak)) extern "C" void __sfuzzer_instrument(uint32_t state_name, uint32_t state_value);
__attribute__((weak)) extern "C" void __sfuzzer_instrument(uint32_t state_name, uint32_t state_value);

using namespace fuzzer;
int main() {
  SM_reset();
  std::cout<<"Start new session: \nIf added state: "<<SM_added_states<<"\n";
  __sfuzzer_instrument(1,1);
  __sfuzzer_instrument(2,1);
  __sfuzzer_instrument(7,1);
  __sfuzzer_instrument(12345,1);
  std::cout<<"----------------------------\n";
  SM_root->print(0);
  std::cout<<"If added state: "<<SM_added_states<<"\n";
  for (auto index : SM_visited_states) {
    std::cout<<"visited state: "<<index->state_name<<"\n";
  }
  for (auto &kv : SM_state_map) {
    std::cout<<kv.first<<" is executed "<<kv.second<<" times"<<std::endl;
  }
  SM_reset();
  std::cout<<"Start new session: \nIf added state: "<<SM_added_states<<"\n";
  __sfuzzer_instrument(3,1);
  __sfuzzer_instrument(4,1);
  std::cout<<"----------------------------\n";
  SM_root->print(0);
  std::cout<<"If added state: "<<SM_added_states<<"\n";
  for (auto index : SM_visited_states) {
    std::cout<<"visited state: "<<index->state_name<<"\n";
  }
  for (auto &kv : SM_state_map) {
    std::cout<<kv.first<<" is executed "<<kv.second<<" times"<<std::endl;
  }
  SM_reset();
  std::cout<<"Start new session: \nIf added state: "<<SM_added_states<<"\n";
  __sfuzzer_instrument(1,1);
  __sfuzzer_instrument(2,1);
  __sfuzzer_instrument(7,2);
  std::cout<<"----------------------------\n";
  SM_root->print(0);
  std::cout<<"If added state: "<<SM_added_states<<"\n";
  for (auto index : SM_visited_states) {
    std::cout<<"visited state: "<<index->state_name<<"\n";
  }
  for (auto &kv : SM_state_map) {
    std::cout<<kv.first<<" is executed "<<kv.second<<" times"<<std::endl;
  }
  SM_reset();
  std::cout<<"Start new session: \nIf added state: "<<SM_added_states<<"\n";
  __sfuzzer_instrument(1,1);
  __sfuzzer_instrument(2,1);
  __sfuzzer_instrument(2,1);
  __sfuzzer_instrument(2,1);
  __sfuzzer_instrument(2,1);
  __sfuzzer_instrument(2,1);
  __sfuzzer_instrument(2,1);
  __sfuzzer_instrument(2,1);
  __sfuzzer_instrument(2,1);
  __sfuzzer_instrument(2,1);
  __sfuzzer_instrument(7,1);
  std::cout<<"----------------------------\n";
  SM_root->print(0);
  std::cout<<"If added state: "<<SM_added_states<<"\n";
  for (auto index : SM_visited_states) {
    std::cout<<"visited state: "<<index->state_name<<"\n";
  }
  for (auto &kv : SM_state_map) {
    std::cout<<kv.first<<" is executed "<<kv.second<<" times"<<std::endl;
  }

  std::cout<<"total node number and hit count.\n";
  std::cout<<SM_num<<"\n";
  std::cout<<SM_root->sum_hits()<<"\n";
}
