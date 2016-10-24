#include "my_vec.h"
#include <iostream>

using namespace std;

void print(my_vec& a) {
	for (int i = 0; i < a.size(); i++)
		cout << a[i] << " ";
	cout << endl;
}

/*int main() {
	my_vec a;
	for (int i = 0; i < 10; i++) {
		a.push_back(i);
		print(a);
	}
	for (int i = 0; i < 10; i++) {
		a.pop_back();
		print(a);
	}
	return 0;
}*/