#include <iostream>
#include <vector>

template <typename T>
int compare(const T &v1, const T &v2)
{
		if (v1 < v2) return -1;
		if (v2 < v1) return 1;
		return 0;
}

int main() 
{
	std::vector<int> vec1{1, 2, 3};
	std::vector<int> vec2{4, 5, 6};

	std::cout << compare(vec1, vec2) << std::endl;	
}
