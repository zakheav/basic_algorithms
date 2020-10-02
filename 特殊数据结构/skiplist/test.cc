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

  skip_list.Delete(1);
  skip_list.Delete(3);
  skip_list.Delete(5);
  skip_list.Delete(7);
  skip_list.Delete(9);
  skip_list.Delete(11);
  skip_list.Delete(0);
  skip_list.Delete(2);
  skip_list.Delete(4);
  skip_list.Delete(6);
  skip_list.Delete(8);
  skip_list.Delete(10);

  skip_list.Delete(12);
  skip_list.Delete(13);
  skip_list.Delete(14);
  skip_list.Delete(15);
  skip_list.Delete(16);
  skip_list.Delete(17);
  skip_list.Delete(18);
  skip_list.Delete(19);

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
