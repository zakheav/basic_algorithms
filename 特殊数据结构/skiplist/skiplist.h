#ifndef SKIPLIST_H_
#define SKIPLIST_H_

#include <vector>
#include <iostream>
#include <random>
#include <unordered_map>

namespace concurrent {

std::default_random_engine generator(time(0));
std::uniform_int_distribution<int32_t> distribution(0, 1);

int32_t Random() {
  return distribution(generator);
}

template<typename T>
struct Node {
  T value_;
  float weight_;
  bool null_;
  Node* down_;
  Node* next_;

  Node(): weight_(0), null_(true), down_(nullptr), next_(nullptr) {}

  Node(T v, float w):
      value_(v), weight_(w), null_(false), down_(nullptr), next_(nullptr) {
  }
};

template<typename T>
class SkipList {
 public:
  SkipList(uint32_t l): level_(l) {
    header_list_.reserve(l);
    for (uint32_t i = 0; i < l; ++i) {
      Node<T>* node = new Node<T>();
      Node<T>* down = i == 0 ? nullptr : header_list_[i - 1];
      node->down_ = down;
      header_list_.push_back(node);
    }
  }

  ~SkipList() {
    for (uint32_t i = 0; i < level_; ++i) {
      for (Node<T>* ptr = header_list_[i]; ptr != nullptr;) {
        Node<T>* nptr = ptr->next_;
        delete ptr;
        ptr = nptr;
      }
    }
  }

  const Node<T>& Get(T v) const;

  void Add(T v, float w);

  void Delete(T v);

  void Print() const;

 private:
  uint32_t level_;
  std::vector<Node<T>*> header_list_;

  std::vector<Node<T>*> GetTrace(T v) const;

  std::vector<std::pair<Node<T>*, Node<T>*>> GetTraceWithPre(T v) const;

  bool Stop() const;
};

template<typename T>
const Node<T>& SkipList<T>::Get(T v) const {
  std::vector<Node<T>*> result = GetTrace(v);
  if (!result.back()->null_ && result.back()->value_ == v) {
    return *result.back();
  }
  throw "node not found";
}

template<typename T>
void SkipList<T>::Add(T v, float w) {
  float origin_w = w;
  std::vector<Node<T>*> result = GetTrace(v);
  Node<T>* pre_insert = nullptr;
  bool stop = false;
  for (int32_t i = result.size() - 1; i >= 0; --i) {
    if (!stop) {
      if (i < result.size() - 1) {
        // calculate sum weight
        Node<T>* end =
            result[i]->next_ == nullptr ?
            nullptr :
            result[i]->next_->down_;
        w = 0;
        for (Node<T>* p = pre_insert; p != end; p = p->next_) {
          w += p->weight_;
        }
      }
      Node<T>* pre = result[i];
      Node<T>* cur = new Node<T>(v, w);
      cur->down_ = pre_insert;
      cur->next_ = pre->next_;
      pre->next_ = cur;
      pre_insert = cur;
      if (i < result.size() - 1) {
        result[i]->weight_ -= w;
      }
      stop = stop || Stop();
    }
    if (i > 0) {
      result[i - 1]->weight_ += origin_w;
    }
  }
}

template<typename T>
void SkipList<T>::Delete(T v) {
  std::vector<std::pair<Node<T>*, Node<T>*>> result = GetTraceWithPre(v);
  // find delete node
  if (!result.back().second->null_ && result.back().second->value_ == v) {
    float origin_w = result.back().second->weight_;
    for (int32_t i = 0; i < result.size(); ++i) {
      if (result[i].second->value_ == v) {
        float new_weight = result[i].second->weight_ - origin_w;
        result[i].first->weight_ += new_weight;
        result[i].first->next_ = result[i].second->next_;
        delete result[i].second;
      } else {
        result[i].second->weight_ -= origin_w;
      }
    }
  }
}

template<typename T>
void SkipList<T>::Print() const {
  for (size_t i = 0; i < header_list_.size(); ++i) {
    int32_t idx = 0;
    for (Node<T>* p = header_list_[i]; p != nullptr; p = p->next_) {
      std::string gap = "  ";
      std::cout << gap << p->value_ << ":" << p->weight_;
    }
    std::cout << std::endl;
  }
}

template<typename T>
std::vector<Node<T>*> SkipList<T>::GetTrace(T v) const {
  std::vector<Node<T>*> result;
  result.reserve(level_);

  uint32_t l = level_ -1;
  Node<T>* cur = header_list_[l];
  Node<T>* pre = nullptr;
  while (cur != nullptr && (cur->null_ || cur->value_ <= v)) {
    if (l == 0) {
      if (cur->null_ || cur->value_ < v) {
        pre = cur;
        cur = cur->next_;
      } else if (cur->value_ == v) {
        result.push_back(cur);
        break;
      } else if (cur->value_ > v) {
        result.push_back(pre);
      }
    } else {
      if (cur->next_ == nullptr || v < cur->next_->value_) {  // v < next
        result.push_back(cur);
        cur = cur->down_;
        --l;
      } else {  // v >= next
        cur = cur->next_;
      }
    }
  }
  if (cur == nullptr) {
    result.push_back(pre);
  }
  return result;
}

template<typename T>
std::vector<std::pair<Node<T>*, Node<T>*>>
SkipList<T>::GetTraceWithPre(T v) const {
  std::vector<std::pair<Node<T>*, Node<T>*>> result;
  result.reserve(level_);

  uint32_t l = level_ -1;
  Node<T>* cur = header_list_[l];
  Node<T>* pre = nullptr;
  while (cur != nullptr && (cur->null_ || cur->value_ <= v)) {
    if (l == 0) {
      if (cur->null_ || cur->value_ < v) {
        pre = cur;
        cur = cur->next_;
      } else if (cur->value_ == v) {
        result.push_back(std::pair<Node<T>*, Node<T>*>(pre, cur));
        break;
      } else if (cur->value_ > v) {
        result.push_back(std::pair<Node<T>*, Node<T>*>(pre, pre));
      }
    } else {
      if (cur->next_ == nullptr || v < cur->next_->value_) {  // v < next
        result.push_back(std::pair<Node<T>*, Node<T>*>(pre, cur));
        pre = pre == nullptr ? nullptr : pre->down_;
        cur = cur->down_;
        while (pre != nullptr && pre->next_ != cur) {
          pre = pre->next_;
        }
        --l;
      } else {  // v >= next
        pre = cur;
        cur = cur->next_;
      }
    }
  }
  if (cur == nullptr) {
    result.push_back(std::pair<Node<T>*, Node<T>*>(pre, pre));
  }
  return result;
}

template<typename T>
bool SkipList<T>::Stop() const {
  return Random() == 1;
}

}  // namespace concurrent

#endif  // SKIPLIST_H_
