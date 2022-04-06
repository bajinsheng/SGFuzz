#include "FuzzerIO.h"
#include "FuzzerStateMachine.h"
#include <vector>
#include <map>
#include <stdio.h>

namespace fuzzer {

  StateMachine::StateMachine(uint32_t n, uint32_t v, StateMachine *p) {
    state_name = n;
    state_value = v;
    parent = p;
    hits = 0;
    SM_num++;
  }

  StateMachine* StateMachine::get_parent() {
    return parent;
  }

  uint64_t StateMachine::get_hits() {
    return hits;
  }

  uint64_t StateMachine::sum_hits() {
    size_t res = hits;
    if (children.size())
      for (auto kv : children) res += kv.second->sum_hits();
    return res;
  }

  size_t StateMachine::get_num_leaves() {
    size_t res = 0;
    if (children.size())
      for (auto kv : children) res += kv.second->get_num_leaves();
    else
      res = 1;
    return res;
  }

  void StateMachine::print(uint32_t indent) {
    uint32_t i = indent;
    while (i--) Printf(" ");
    Printf("%d:%d:%d:%d\n", state_name, state_value, hits, children.size());
    if (children.size()) {
      for (auto kv : children) {
        kv.second->print(indent + 2);
      }
    }
  }

  robin_hood::unordered_map<uint32_t, uint32_t> SM_state_map;
  static pthread_mutex_t state_map_mutex = PTHREAD_MUTEX_INITIALIZER;
  static pthread_mutex_t state_tree_mutex = PTHREAD_MUTEX_INITIALIZER;

  void SM_reset() {
    SM_added_states = false;
    pthread_mutex_lock(&state_tree_mutex);
    SM_visited_states.clear();
    pthread_mutex_unlock(&state_tree_mutex);
    pthread_mutex_lock(&state_map_mutex);
    SM_state_map.clear();
    pthread_mutex_unlock(&state_map_mutex);
    SM_cur_node = SM_root;
  }

  StateMachine *SM_root = new StateMachine(0, 0, nullptr);
  StateMachine *SM_cur_node = SM_root;
  uint32_t SM_num = 0;
  Vector<StateMachine*> SM_visited_states;
  
  bool SM_added_states = false;

  extern "C" void __sfuzzer_instrument(uint32_t state_name, uint32_t state_value) {
    uint32_t state_id = (state_name << 8) + state_value;

    // The State machine is not initialized yet.
    // It is usually because the instrument fucntion is called  in __libc_csu_init
    if (SM_cur_node == nullptr) {
      return;
    }

    // Avoid loop
    pthread_mutex_lock(&state_map_mutex);
    robin_hood::unordered_map<uint32_t, uint32_t>::iterator SM_state_map_iterator = SM_state_map.find(state_id);
    if (SM_state_map_iterator == SM_state_map.end()) {
      SM_state_map.insert(robin_hood::unordered_map<uint32_t, uint32_t>::value_type(state_id, 1));
    }
    else if (SM_state_map_iterator->second >= 2) {
      pthread_mutex_unlock(&state_map_mutex);
      return;
    }
    else {
      SM_state_map_iterator->second++;
    }
    pthread_mutex_unlock(&state_map_mutex);

    // Merge to the global state machine
    pthread_mutex_lock(&state_tree_mutex);
    robin_hood::unordered_map<uint32_t, StateMachine*>::iterator SM_cur_node_children_iterator = SM_cur_node->children.find(state_id);
    if (SM_cur_node_children_iterator == SM_cur_node->children.end()) {
      StateMachine *new_node = new StateMachine(state_name, state_value, SM_cur_node);
      SM_added_states = true;
      new_node->hits = 1;
      SM_cur_node->children.insert(robin_hood::unordered_map<uint32_t, StateMachine*>::value_type(state_id, new_node));
      SM_cur_node = new_node;
    }
    else {
      SM_cur_node_children_iterator->second->hits++;
      SM_cur_node = SM_cur_node_children_iterator->second;
    }
    SM_visited_states.push_back(SM_cur_node);
    pthread_mutex_unlock(&state_tree_mutex);
  
  }

}