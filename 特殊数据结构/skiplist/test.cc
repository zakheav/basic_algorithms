#include <iostream>

#include "skiplist.h"

using namespace concurrent;

int main() {
  SkipList<int32_t> skip_list(4);

  for (int32_t i = 0; i < 20; i += 1) {
    skip_list.Add(i, 1);
  }
  for (int32_t i = 0; i < 20; i += 2) {
    skip_list.Add(i, 1);
  }
  skip_list.Print();

  for (int32_t i = 0; i < 20; ++i) {
    try {
      const Node<int32_t>& n = skip_list.Get(i);
      std::cout << n.value_ << std::endl;
    } catch(const char* msg) {
      std::cout << msg << std::endl;
    }
  }
  return 0;
}
