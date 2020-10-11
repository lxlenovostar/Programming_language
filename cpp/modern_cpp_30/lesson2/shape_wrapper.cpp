#include <stdlib.h>
#include <iostream>

using namespace std;

class shape {
 public:
  void print() {
    std::cout << "hi shape\n";
  }

private:
   int a;
};

template <typename T>
class smart_ptr {
  public:
    explicit smart_ptr(T* ptr = nullptr): ptr_(ptr) {
      std::cout << "smart_ptr" << std::endl;
    }
    ~smart_ptr() {
      std::cout << "~smart_ptr" << std::endl;
      delete ptr_;
    }

    T* get() const { return ptr_; }

    T& operator*() const { return *ptr_; }
    T* operator->() const { return ptr_; }
    operator bool() const { return ptr_; }

    smart_ptr(const smart_ptr&)
        = delete;

    smart_ptr& operator=(const smart_ptr&)
        = delete;

 private:
    T* ptr_;
};

void foo() {
  shape *obj = new shape();
  /*
  smart_ptr<shape> auto_ptr(obj);
  auto_ptr->print();
  */

  smart_ptr<shape> ptr1{obj};
  ptr1->print();
  smart_ptr<shape> ptr2{ptr1};
  ptr2->print();
}

int main() {
    foo();
    return 0;
}

