#include <iterator>
#include <iostream>
#include <vector>
#include <functional>
#include <cassert>
#include <list>
#include <chrono>
#include <exception>
#include <string>
#include <algorithm>

//Напишите реализацию для класса BiDirectionalList и тесты к нему.
//
//Предусмотрите обработку ошибок (выход за границы массива, передача неверного
//итератора в метод и т. д.) с использованием механизма исключений.
//
//-----------------------------------------------------------------------------

template<typename T>
class BiDirectionalList {
 protected:
  struct Node;

 public:
  class Iterator : public std::iterator<std::bidirectional_iterator_tag, T> {
   public:
    T& operator*() const;
    T* operator->() const;

    Iterator& operator++();
    const Iterator operator++(int);

    Iterator& operator--();
    const Iterator operator--(int);

    bool operator==(const Iterator& other) const;
    bool operator!=(const Iterator& other) const;

   private:
    friend class BiDirectionalList;

    const BiDirectionalList* const list_;
    Node* node_;

    Iterator(const BiDirectionalList* const list, Node* node) : list_(list),
                                                                node_(node) {}
  };

  class ConstIterator :
      public std::iterator<std::bidirectional_iterator_tag, T> {
   public:
    const T& operator*() const;
    const T* operator->() const;

    ConstIterator& operator++();
    const ConstIterator operator++(int);

    ConstIterator& operator--();
    const ConstIterator operator--(int);

    bool operator==(const ConstIterator& other) const;
    bool operator!=(const ConstIterator& other) const;

   private:
    friend class BiDirectionalList;

    const BiDirectionalList* const list_;
    const Node* node_;

    ConstIterator(const BiDirectionalList* const list, Node* node)
        : list_(list), node_(node) {}
  };

  BiDirectionalList() : first_(nullptr), last_(nullptr) {}

  ~BiDirectionalList() { Clear(); }

  bool IsEmpty() const;

  void Clear();

  Iterator begin();
  Iterator end();

  ConstIterator begin() const;
  ConstIterator end() const;

  std::vector<T> AsArray() const;

  void InsertBefore(Iterator position, const T& value);
  void InsertBefore(Iterator position, T&& value);

  void InsertAfter(Iterator position, const T& value);
  void InsertAfter(Iterator position, T&& value);

  void PushBack(const T& value);
  void PushBack(T&& value);

  void PushFront(const T& value);
  void PushFront(T&& value);

  void Erase(Iterator position);

  void PopFront();
  void PopBack();

  Iterator Find(const T& value);
  ConstIterator Find(const T& value) const;

  Iterator Find(std::function<bool(const T&)> predicate);
  ConstIterator Find(std::function<bool(const T&)> predicate) const;

 protected:
  struct Node {
    explicit Node(const T& value);
    explicit Node(T&& value);

    T value_;
    Node* next_node_;
    Node* previous_node_;
  };

  Node* first_;
  Node* last_;

  void InsertBefore(Node* existing_node, Node* new_node);
  void InsertAfter(Node* existing_node, Node* new_node);
  void Erase(Node* node);
};

template<typename T>
BiDirectionalList<T>::Node::Node(const T& value) : value_(value),
                                                   previous_node_(nullptr),
                                                   next_node_(nullptr) {}
template<typename T>
BiDirectionalList<T>::Node::Node(T&& value) : value_(value),
                                              previous_node_(nullptr),
                                              next_node_(nullptr) {
  value = T();
}

template<typename T>
T& BiDirectionalList<T>::Iterator::operator*() const {
  return node_->value_;
}
template<typename T>
T* BiDirectionalList<T>::Iterator::operator->() const {
  return &node_->value_;
}

template<typename T>
typename BiDirectionalList<T>::Iterator& BiDirectionalList<T>::
    Iterator::operator++() {
  if (node_ == nullptr) {
    throw std::runtime_error("Impossible to increase iterator");
  }
  node_ = node_->next_node_;
  return *this;
}
template<typename T>
const typename BiDirectionalList<T>::Iterator BiDirectionalList<T>::
    Iterator::operator++(int) {
  if (node_ == nullptr) {
    throw std::runtime_error("Impossible to increase iterator");
  }
  auto new_node = node_;
  node_ = node_->next_node_;
  Iterator new_iterator(list_, new_node);
  return new_iterator;
}

template<typename T>
typename BiDirectionalList<T>::Iterator& BiDirectionalList<T>::
    Iterator::operator--() {
  if (node_ == list_->first_) {
    throw std::runtime_error("Impossible to reduce iterator");
  } else if (node_ == nullptr) {
    node_ = list_->last_;
  } else {
    node_ = node_->previous_node_;
  }
  return *this;
}
template<typename T>
const typename BiDirectionalList<T>::Iterator BiDirectionalList<T>::
    Iterator::operator--(int) {
  if (node_ == list_->first_) {
    throw std::runtime_error("Impossible to reduce iterator");
  }
  auto new_node = node_;
  if (node_ == nullptr) {
    node_ = list_->last_;
  } else {
    node_ = node_->previous_node_;
  }
  Iterator new_iterator(list_, new_node);
  return new_iterator;
}

template<typename T>
bool BiDirectionalList<T>::Iterator::operator==
  (const BiDirectionalList::Iterator& other) const {
  return other.node_ == node_;
}
template<typename T>
bool BiDirectionalList<T>::Iterator::operator!=
  (const BiDirectionalList::Iterator& other) const {
  return other.node_ != node_;
}

template<typename T>
const T& BiDirectionalList<T>::ConstIterator::operator*() const {
  return node_->value_;
}
template<typename T>
const T* BiDirectionalList<T>::ConstIterator::operator->() const {
  return &node_->value_;
}

template<typename T>
typename BiDirectionalList<T>::ConstIterator& BiDirectionalList<T>::
    ConstIterator::operator++() {
  if (node_ == nullptr) {
    throw std::runtime_error("Impossible to increase iterator");
  }
  node_ = node_->next_node_;
  return *this;
}
template<typename T>
const typename BiDirectionalList<T>::ConstIterator BiDirectionalList<T>::
    ConstIterator::operator++(int) {
  if (node_ == nullptr) {
    throw std::runtime_error("Impossible to increase iterator");
  }
  auto new_node = node_;
  node_ = node_->next_node_;
  ConstIterator new_iterator(list_, new_node);
  return new_iterator;
}

template<typename T>
typename BiDirectionalList<T>::ConstIterator& BiDirectionalList<T>::
    ConstIterator::operator--() {
  if (node_ == list_->first_) {
    throw std::runtime_error("Impossible to reduce iterator");
  }
  if (node_ == nullptr) {
    node_ = list_->last_;
  } else {
    node_ = node_->previous_node_;
  }
  return *this;
}
template<typename T>
const typename BiDirectionalList<T>::ConstIterator BiDirectionalList<T>::
    ConstIterator::operator--(int) {
  if (node_ == list_->first_) {
    throw std::runtime_error("Impossible to reduce iterator");
  }
  auto new_node = node_;
  if (node_ == nullptr) {
    node_ = list_->last_;
  } else {
    node_ = node_->previous_node_;
  }
  ConstIterator new_iterator(list_, new_node);
  return new_iterator;
}

template<typename T>
bool BiDirectionalList<T>::ConstIterator::operator==
  (const BiDirectionalList::ConstIterator& other) const {
  return other.node_ == node_;
}
template<typename T>
bool BiDirectionalList<T>::ConstIterator::operator!=
  (const BiDirectionalList::ConstIterator& other) const {
  return other.node_ != node_;
}

template<typename T>
bool BiDirectionalList<T>::IsEmpty() const {
  return last_ == first_ && last_ == nullptr;
}

template<typename T>
void BiDirectionalList<T>::Clear() {
  while (!IsEmpty()) {
    Erase(last_);
  }
}

template<typename T>
typename BiDirectionalList<T>::Iterator BiDirectionalList<T>::begin() {
  return BiDirectionalList::Iterator(this, first_);
}
template<typename T>
typename BiDirectionalList<T>::Iterator BiDirectionalList<T>::end() {
  return BiDirectionalList::Iterator(this, nullptr);
}

template<typename T>
typename BiDirectionalList<T>::ConstIterator BiDirectionalList<T>::
    begin() const {
  return BiDirectionalList::ConstIterator(this, first_);
}
template<typename T>
typename BiDirectionalList<T>::ConstIterator BiDirectionalList<T>::
    end() const {
  return BiDirectionalList::ConstIterator(this, nullptr);
}

template<typename T>
std::vector<T> BiDirectionalList<T>::AsArray() const {
  std::vector<T> new_vector;
  for (auto i = begin(); i != end(); ++i) {
    new_vector.push_back(*i);
  }
  return new_vector;
}

template<typename T>
void BiDirectionalList<T>::InsertBefore(BiDirectionalList::Iterator position,
                                        const T& value) {
  Node* new_node = new Node(value);
  InsertBefore(position.node_, new_node);
}
template<typename T>
void BiDirectionalList<T>::InsertBefore(BiDirectionalList::Iterator position,
                                        T&& value) {
  Node* new_node = new Node(std::move(value));
  InsertBefore(position.node_, new_node);
}

template<typename T>
void BiDirectionalList<T>::InsertAfter(BiDirectionalList::Iterator position,
                                       const T& value) {
  Node* new_node = new Node(value);
  InsertAfter(position.node_, new_node);
}
template<typename T>
void BiDirectionalList<T>::InsertAfter(BiDirectionalList::Iterator position,
                                       T&& value) {
  Node* new_node = new Node(std::move(value));
  InsertAfter(position.node_, new_node);
}

template<typename T>
void BiDirectionalList<T>::PushBack(const T& value) {
  Node* new_node = new Node(value);
  InsertAfter(last_, new_node);
}
template<typename T>
void BiDirectionalList<T>::PushBack(T&& value) {
  Node* new_node = new Node(std::move(value));
  InsertAfter(last_, new_node);
}

template<typename T>
void BiDirectionalList<T>::PushFront(const T& value) {
  Node* new_node = new Node(value);
  InsertBefore(first_, new_node);
}
template<typename T>
void BiDirectionalList<T>::PushFront(T&& value) {
  Node* new_node = new Node(std::move(value));
  InsertBefore(first_, new_node);
}

template<typename T>
void BiDirectionalList<T>::Erase(BiDirectionalList::Iterator position) {
  if (IsEmpty()) {
    throw std::runtime_error("Impossible to delete element from empty list");
  }
  if (position == end()) {
    throw std::runtime_error("Impossible to delete end");
  }
  Erase(position.node_);
}

template<typename T>
void BiDirectionalList<T>::PopFront() {
  if (IsEmpty()) {
    throw std::runtime_error("Impossible to delete element from empty list");
  }
  Erase(begin().node_);
}
template<typename T>
void BiDirectionalList<T>::PopBack() {
  if (IsEmpty()) {
    throw std::runtime_error("Impossible to delete element from empty list");
  }
  Erase((--end()).node_);
}

template<typename T>
typename BiDirectionalList<T>::Iterator BiDirectionalList<T>::
Find(const T& value) {
  for (Iterator i = begin(); i != end(); ++i) {
    if (*i == value) {
      return i;
    }
  }
  return end();
}
template<typename T>
typename BiDirectionalList<T>::ConstIterator BiDirectionalList<T>::
Find(const T& value) const {
  for (ConstIterator i = begin(); i != end(); ++i) {
    if (*i == value) {
      return i;
    }
  }
  return end();
}

template<typename T>
typename BiDirectionalList<T>::Iterator BiDirectionalList<T>::
Find(std::function<bool(const T&)> predicate) {
  for (Iterator i = begin(); i != end(); ++i) {
    if (predicate(*i)) {
      return i;
    }
  }
  return end();
}
template<typename T>
typename BiDirectionalList<T>::ConstIterator BiDirectionalList<T>::
Find(std::function<bool(const T&)> predicate) const {
  for (ConstIterator i = begin(); i != end(); ++i) {
    if (predicate(*i)) {
      return i;
    }
  }
  return end();
}

template<typename T>
void BiDirectionalList<T>::InsertBefore(BiDirectionalList::Node* existing_node,
                                        BiDirectionalList::Node* new_node) {
  if (first_ == nullptr) {
    first_ = last_ = new_node;
  } else if (existing_node == first_) {
    new_node->next_node_ = existing_node;
    existing_node->previous_node_ = new_node;
    first_ = new_node;
  } else {
    Node* existing_previous_node = existing_node->previous_node_;
    existing_node->previous_node_ = new_node;
    new_node->previous_node_ = existing_previous_node;
    existing_previous_node->next_node_ = new_node;
    new_node->next_node_ = existing_node;
  }
}
template<typename T>
void BiDirectionalList<T>::InsertAfter(BiDirectionalList::Node* existing_node,
                                       BiDirectionalList::Node* new_node) {
  if (IsEmpty()) {
    first_ = last_ = new_node;
  } else if (existing_node == last_) {
    existing_node->next_node_ = new_node;
    new_node->previous_node_ = existing_node;
    last_ = new_node;
  } else {
    Node* existing_next_node = existing_node->next_node_;
    existing_node->next_node_ = new_node;
    new_node->next_node_ = existing_next_node;
    existing_next_node->previous_node_ = new_node;
    new_node->previous_node_ = existing_node;
  }
}
template<typename T>
void BiDirectionalList<T>::Erase(BiDirectionalList::Node* node) {
  if (node->next_node_ == nullptr && node->previous_node_ == nullptr) {
    delete node;
    first_ = last_ = nullptr;
  } else if (node->next_node_ == nullptr) {
    Node* prev = node->previous_node_;
    delete node;
    last_ = prev;
    last_->next_node_ = nullptr;
  } else if (node->previous_node_ == nullptr) {
    Node* next = node->next_node_;
    delete node;
    first_ = next;
    first_->previous_node_ = nullptr;
  } else {
    Node* prev = node->previous_node_;
    Node* next = node->next_node_;
    prev->next_node_ = next;
    next->previous_node_ = prev;
    delete node;
  }
}

// Для тестирования группы закомментируйте или удалите строчку
// "#define SKIP_XXXXX" для соответствующей группы тестов.
//
// #define SKIP_Actions_with_iterators
// #define SKIP_PushFront
// #define SKIP_PushBack
// #define SKIP_PopBack
// #define SKIP_PopFront
// #define SKIP_Find
// #define SKIP_Find_with_predicate
// #define SKIP_Insert
// #define SKIP_Erase
// #define SKIP_Combo_vombo
// #define SKIP_Exception
//
//===========================================================

int main() {
  int const COUNT = 15;
  srand(time(0));
#ifndef SKIP_Actions_with_iterators
  {
    std::list<int> true_list;
    BiDirectionalList<int> my_list;
    for (int i = 0; i < COUNT; i++) {
      int temp = rand();
      true_list.push_back(temp);
      my_list.PushBack(temp);
    }
    BiDirectionalList<int>::Iterator iter = my_list.begin();
    auto true_iter = true_list.begin();
    BiDirectionalList<int>::Iterator iter2 = --my_list.end();
    auto true_iter2 = --true_list.end();
    assert(*true_iter2 == *iter2);
    for (int i = 0; i < COUNT; i++) {
      assert(*iter == *true_iter);
      if (i != COUNT - 1) {
        iter++;
        true_iter++;
      }
    }
    for (int i = 0; i < COUNT; i++) {
      assert(*iter == *true_iter);
      if (i != COUNT - 1) {
        iter--;
        true_iter--;
      }
    }
    for (int i = 0; i < COUNT; i++) {
      assert(*iter == *true_iter);
      if (i != COUNT - 1) {
        ++iter;
        ++true_iter;
      }
    }
    for (int i = 0; i < COUNT; i++) {
      assert(*iter == *true_iter);
      if (i != COUNT - 1) {
        --iter;
        --true_iter;
      }
    }
    my_list.Clear();
    assert(my_list.IsEmpty());
    std::cout << "[PASS] Actions with iterators" << std::endl;
  }
#else
  std::cout << "[SKIPPED] Actions with iterators" << std::endl;
#endif // SKIP_Actions_with_iterators

#ifndef SKIP_PushFront
  {
    BiDirectionalList<int> my_list;
    std::vector<int> checker;
    assert(my_list.IsEmpty());
    for (int i = 0; i < COUNT; i++) {
      int temp = rand();
      my_list.PushFront(temp);
      checker.push_back(temp);
    }
    reverse(begin(checker), end(checker));
    assert(!(my_list.IsEmpty()));
    assert(checker == my_list.AsArray());
    my_list.Clear();
    assert(my_list.IsEmpty());
    std::cout << "[PASS] PushFront" << std::endl;
  }
#else
  std::cout << "[SKIPPED] PushFront" << std::endl;
#endif // SKIP_PushFront

#ifndef SKIP_PushBack
  {
    BiDirectionalList<int> my_list;
    std::vector<int> checker;
    assert(my_list.IsEmpty());
    for (int i = 0; i < COUNT; i++) {
      int temp = rand();
      my_list.PushBack(temp);
      checker.push_back(temp);
    }
    assert(!(my_list.IsEmpty()));
    assert(checker == my_list.AsArray());
    my_list.Clear();
    assert(my_list.IsEmpty());
    std::cout << "[PASS] PushBack" << std::endl;
  }
#else
  std::cout << "[SKIPPED] PushBack" << std::endl;
#endif // SKIP_PushBack

#ifndef SKIP_PopBack
  {
    BiDirectionalList<int> my_list;
    std::vector<int> checker;
    assert(my_list.IsEmpty());
    for (int i = 0; i < COUNT; i++) {
      int temp = rand();
      my_list.PushBack(temp);
      checker.push_back(temp);
    }
    for (int i = 0; i < COUNT; i++) {
      checker.pop_back();
      my_list.PopBack();
      assert(checker == my_list.AsArray());
    }
    assert(my_list.IsEmpty());
    std::cout << "[PASS] PopBack" << std::endl;
  }
#else
  std::cout << "[SKIPPED PopBack]" << std::endl;
#endif // SKIP_PopBack

#ifndef SKIP_PopFront
  {
    BiDirectionalList<int> my_list;
    std::vector<int> checker;
    assert(my_list.IsEmpty());
    for (int i = 0; i < COUNT; i++) {
      int temp = rand();
      my_list.PushBack(temp);
      checker.push_back(temp);
    }
    for (int i = 0; i < COUNT; i++) {
      checker.erase(checker.begin());
      my_list.PopFront();
      assert(checker == my_list.AsArray());
    }
    assert(my_list.IsEmpty());
    std::cout << "[PASS] PopFront" << std::endl;
  }
#else
  std::cout << "[SKIPPED] PopFront" << std::endl;
#endif // SKIP_PopFront

#ifndef SKIP_Find
  {
    BiDirectionalList<int> my_list;
    for (int i = 0; i < COUNT; i++) {
      my_list.PushBack(i);
    }
    BiDirectionalList<int>::Iterator check1 = my_list.Find(17);
    BiDirectionalList<int>::Iterator check2 = my_list.Find(2);
    BiDirectionalList<int>::Iterator check3 = my_list.Find(10);
    BiDirectionalList<int>::Iterator check4 = my_list.Find(7);
    BiDirectionalList<int>::Iterator check5 = my_list.Find(25);
    assert(check1 == my_list.end());
    assert(*check2 == 2);
    assert(*check4 == 7);
    assert(*check3 == 10);
    assert(check5 == my_list.end());
    std::cout << "[PASS] Find" << std::endl;
    my_list.Clear();
    assert(my_list.IsEmpty());
  }
#else
  std:: cout << "[SKIPPED] Find" << std::endl;
#endif // SKIP_Find

#ifndef SKIP_Find_with_predicate
  {
    BiDirectionalList<int> my_list;
    for (int i = 0; i < COUNT; i++) {
      my_list.PushBack(i);
    }
    auto function1 = [](const int &value) {
      return value == 5;
    };
    auto function2 = [](const int &value) {
      return value == 10;
    };
    auto function3 = [](const int &value) {
      return value == 0;
    };
    auto function4 = [](const int &value) {
      return value == -1;
    };
    auto function5 = [](const int &value) {
      return value == 18;
    };
    BiDirectionalList<int>::Iterator check1 = my_list.Find(function1);
    BiDirectionalList<int>::Iterator check2 = my_list.Find(function2);
    BiDirectionalList<int>::Iterator check3 = my_list.Find(function3);
    BiDirectionalList<int>::Iterator check4 = my_list.Find(function4);
    BiDirectionalList<int>::Iterator check5 = my_list.Find(function5);
    assert(*check1 == 5);
    assert(*check2 == 10);
    assert(*check3 == 0);
    assert(check4 == my_list.end());
    assert(check5 == my_list.end());
    my_list.Clear();
    assert(my_list.IsEmpty());
    std::cout << "[PASS] Find with predicate" << std::endl;
  }
#else
  std::cout << "[SKIPPED] Find with predicate" << std::endl;
#endif // SKIP_Find_with_predicate

#ifndef SKIP_Insert
  {
    BiDirectionalList<int> my_list;
    std::vector<int> checker;
    for (int i = 0; i < 10; i++) {
      my_list.PushBack(i * 2 + 1);
      checker.push_back(i * 2 + 1);
    };
    assert(checker == my_list.AsArray());
    checker = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 11, 13, 15, 17, 19};
    for (int i = 0; i < 5; i++) {
      BiDirectionalList<int>::Iterator check = my_list.Find(i * 2 + 1);
      my_list.InsertBefore(check, 2 * i);
    }
    assert(checker == my_list.AsArray());
    for (int i = 5; i < 10; i++) {
      BiDirectionalList<int>::Iterator check = my_list.Find(i * 2 - 1);
      my_list.InsertAfter(check, 2 * i);
    }
    checker = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
               11, 12, 13, 14, 15, 16, 17, 18, 19};
    assert(checker == my_list.AsArray());
    my_list.Clear();
    std::cout << "[PASS] Insert" << std::endl;
  }
#else
  std::cout << "[SKIPPED] Insert" << std::endl;
#endif // SKIP_Insert

#ifndef SKIP_Erase
  {
    BiDirectionalList<int> my_list;
    std::vector<int> checker;
    for (int i = 0; i < 20; i++) {
      my_list.PushBack(i);
      checker.push_back(i);
    }
    assert(checker == my_list.AsArray());
    for (int i = 0; i < 10; i++) {
      BiDirectionalList<int>::Iterator check = my_list.Find(i * 2);
      my_list.Erase(check);
    }
    checker = {1, 3, 5, 7, 9, 11, 13, 15, 17, 19};
    assert(checker == my_list.AsArray());
    for (int i = 0; i < 5; i++) {
      BiDirectionalList<int>::Iterator check = my_list.Find(i * 2 + 1);
      my_list.Erase(check);
    }
    checker = {11, 13, 15, 17, 19};
    assert(checker == my_list.AsArray());
    my_list.Clear();
    std::cout << "[PASS] Erase" << std::endl;
  }
#else
  std::cout << "[SKIPPED] Erase" << std::endl;
#endif // SKIP_Erase

#ifndef SKIP_Combo_vombo
  {
    BiDirectionalList<int> my_list;
    std::vector<int> checker;
    for (int i = 0; i < COUNT; i++) {
      my_list.PushFront(i);
      checker.push_back(i);
    }
    reverse(begin(checker), end(checker));
    assert(checker == my_list.AsArray());
    for (int i = 0; i < COUNT; i++) {
      my_list.PopFront();
      checker.erase(begin(checker));
      assert(checker == my_list.AsArray());
    }
    for (int i = 0; i < COUNT; i++) {
      my_list.PushBack(i);
      checker.push_back(i);
    }
    std::vector<int> checker2;
    for (int i = 0; i < COUNT; i++) {
      my_list.InsertBefore(my_list.begin(), (-1) * i);
      checker2.push_back((-1) * i);
    }
    reverse(begin(checker2), end(checker2));
    checker2.insert(end(checker2), begin(checker), end(checker));
    assert(checker2 == my_list.AsArray());
    for (int i = 0; i < COUNT; i++) {
      my_list.InsertAfter(--my_list.end(), i + COUNT);
      checker2.push_back(i + COUNT);
    }
    assert(checker2 == my_list.AsArray());
    std::cout << "[PASS] Combo_vombo" << std::endl;
  }
#else
  std::cout << "[SKIPPED] Combo_vombo" << std::endl;
#endif // SKIP_Combo_vombo

#ifndef SKIP_Exception
  {
    BiDirectionalList<double> my_list;
    try {
      my_list.Erase(my_list.begin());
    } catch (std::runtime_error &ex) {
      assert(static_cast<std::string>(ex.what()) ==
          "Impossible to delete element from empty list");
    }
    my_list.PushBack(8.5);
    BiDirectionalList<double>::Iterator iter = my_list.begin();
    try {
      my_list.Erase(my_list.end());
    } catch (std::runtime_error &ex) {
      assert(static_cast<std::string>(ex.what()) ==
          "Impossible to delete end");
    }
    try {
      iter--;
    } catch (std::runtime_error &ex) {
      assert(static_cast<std::string>(ex.what()) ==
          "Impossible to reduce iterator");
    }
    BiDirectionalList<double>::Iterator iter2 = my_list.end();
    try {
      --iter2;
    } catch (std::runtime_error &ex) {
      assert(static_cast<std::string>(ex.what()) ==
          "Impossible to reduce iterator");
    }
    BiDirectionalList<double>::Iterator iter3 = my_list.end();
    try {
      iter3++;
    } catch (std::runtime_error &ex) {
      assert(static_cast<std::string>(ex.what()) ==
          "Impossible to increase iterator");
    }
    BiDirectionalList<double>::Iterator iter4 = my_list.end();
    try {
      ++iter4;
    } catch (std::runtime_error &ex) {
      assert(static_cast<std::string>(ex.what()) ==
          "Impossible to increase iterator");
    }
    std::cout << "[PASS] Exception" << std::endl;
  }
#else
  std::cout << "[SKIPPED] Exception" << std::endl;
#endif // SKIP_Exception

  return 0;
}
