#include <iostream>
#include <jsoncpp/json/json.h> // or something

using namespace std;

int main() {
	Json::Value val;
    cin >> val;

    Json::FastWriter fast;
    Json::StyledWriter styled;
    string sFast = fast.write(val);
    string sStyled = styled.write(val);

    cout << "Fast:\n" << sFast << "Styled:\n" << sStyled;
	cout << "Styled stream:\n";
	
	Json::StyledStreamWriter styledStream;
    styledStream.write(cout, val);
}
