#include <iostream>
#include "mahjong.cpp"
using namespace std;

int main() {
	string s;
	cin >> s;
	Mahjong m(s);
	cout << m.getScore();
	return 0;
}
