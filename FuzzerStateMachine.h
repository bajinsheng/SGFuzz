#include "FuzzerDefs.h"
#include "robin_hood.h"


namespace fuzzer {


  class StateMachine {
    public:

      StateMachine(uint32_t state_name, uint32_t state_value, StateMachine *p);
      ~StateMachine();

      StateMachine* get_parent();
      uint64_t get_hits();
      size_t sum_hits();
      size_t get_num_leaves();
      std::string get_id();
      void print(uint32_t indent);

      uint32_t state_name;
      uint32_t state_value;
      uint64_t hits;
      StateMachine *parent;
      robin_hood::unordered_map<uint32_t, StateMachine*> children;
  };
  extern robin_hood::unordered_map<uint32_t, uint32_t> SM_state_map;
  extern uint32_t SM_num;
  extern StateMachine *SM_root;
  extern StateMachine *SM_cur_node;
  extern Vector<StateMachine*> SM_visited_states;
  extern bool SM_added_states;

  void SM_reset();
}
