#include <iostream>
#include <string>

using namespace std;

class Test
{
	public:
		Test(string s) {
			str = s;
		    cout<<"Test creat\n";
		}
					    
		~Test() {
			cout << "Test delete:" << str<< endl;
		}
						    
		string& getStr() {
			return str;
		}
							    
		void setStr(string s) {
			str = s;
		}
								    
		void print() {
			cout<< str << endl;
		}

		private:
			string str;
};

std::unique_ptr <Test> fun()
{
	return unique_ptr<Test>(new Test("789"));
}
 
int main()
{
	unique_ptr<Test> ptest(new Test("123"));
	unique_ptr<Test> ptest2(new Test("456"));
	ptest->print();
	ptest2 = std::move(ptest);//不能直接ptest2 = ptest

	if(ptest == NULL) 
		cout << "ptest = NULL\n";
		   
	Test* p = ptest2.release();
	p->print();
	ptest.reset(p);
	ptest->print();
	ptest2 = fun(); //这里可以用=，因为使用了移动构造函数
	ptest2->print();
	return 0;
}
