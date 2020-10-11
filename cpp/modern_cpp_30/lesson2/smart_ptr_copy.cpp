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

    //smart_ptr(smart_ptr& other)  {
    //smart_ptr(smart_ptr&& other)  {
    template <typename U>
    smart_ptr(smart_ptr<U>&& other)  {
      ptr_ = other.release();
    }

    //smart_ptr& operator=(smart_ptr& rhs)  {
    smart_ptr& operator=(smart_ptr rhs)  {
      //首先rhs把自己维护的指针交给给临时对象smart_ptr(rhs)，然后这个临时对象维护的指针和this对象维护的指针交换一下，this对象就拿到rhs维护的指针了，临时对象smart_ptr拿到this之前维护的指针，它会随着临时对象smart_ptr销毁而被delete。
      //smart_ptr(rhs).swap(*this);
      rhs.swap(*this);
      return *this;
    }

    T* release()  {
      T* ptr = ptr_;
      ptr_ = nullptr;
      return ptr;
    }

    void swap(smart_ptr& rhs)  {
      using std::swap;
      swap(ptr_, rhs.ptr_);
    }

 private:
    T* ptr_;
};

void foo() {
  shape *obj = new shape();
  /*
  smart_ptr<shape> auto_ptr(obj);
  auto_ptr->print();
  */

  /*
  smart_ptr<shape> ptr1{obj};
  ptr1->print();
  smart_ptr<shape> ptr2{ptr1};
  ptr2->print();
  */


  smart_ptr<shape> ptr1{obj};
  //smart_ptr<shape> ptr2{ptr1};             // 编译出错
  smart_ptr<shape> ptr3;
  //ptr3 = ptr1;                             // 编译出错
  ptr3 = std::move(ptr1);                  // OK，可以
  smart_ptr<shape> ptr4{std::move(ptr3)};  // OK，可以
}

int main() {
    foo();
    return 0;
}

