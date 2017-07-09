#include "ex14_07.h"
#include <algorithm>
#include <iostream>

std::pair<char*, char*> String::alloc_n_copy(const char* b, const char* e)
{
	//分配内存单元
    auto str = alloc.allocate(e - b);
	//uninitialized_copy(beg, end, mem) 以[beg:end]的各个元素初始化mem开始的各个元素
	//Return value: Iterator to the element past the last element copied.
    return {str, std::uninitialized_copy(b, e, str)};
}

void String::range_initializer(const char* first, const char* last)
{
    auto newstr = alloc_n_copy(first, last);
    elements = newstr.first;
    end = newstr.second;
}

String::String(const char* s)
{
	//C++11 新标准引入auto类型说明符，用它就能让编译器替我们去分析表达式所属的类型
    auto sl = s;
    while (*sl) ++sl;
    range_initializer(s, ++sl);
}

String::String(const String& rhs)
{
    range_initializer(rhs.elements, rhs.end);
    std::cout << "copy constructor" << std::endl;
}

void String::free()
{
    if (elements) {
        std::for_each(elements, end, [this](char& c) { alloc.destroy(&c); });
		//回收elements所指的内存空间
        alloc.deallocate(elements, end - elements);
    }
}

String::~String()
{
    free();
}

String& String::operator=(const String& rhs)
{
    auto newstr = alloc_n_copy(rhs.elements, rhs.end);
    free();
    elements = newstr.first;
    end = newstr.second;
    std::cout << "copy-assignment" << std::endl;
    return *this;
}

std::ostream& operator<<(std::ostream& os, const String& s)
{
    auto c = s.c_str();
    while (*c) os << *c++;
    return os;
}
