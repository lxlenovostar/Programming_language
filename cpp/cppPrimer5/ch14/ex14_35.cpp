#include <iostream>
#include <string>

using namespace std;

class WriteString {
	public:
		WriteString(istream &i = cin): is(i) {}
		void operator()() {
			std::string s;
			is >> s;
			is ? (cout << s << endl) : (cout << string() << endl);
		}
	
	private:
		istream &is;

};

int main(void)
{
	WriteString w;
	w();
}
