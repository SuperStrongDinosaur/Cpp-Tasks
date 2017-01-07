#include "bind.h"
#include <iostream>

using std::cout;
using std::endl;

int plain_inc(int a) {
	return ++a;
}

int ref_inc(int& a) {
	return ++a;
}

int cosnt_ref_inc(int const& a) {
	return a + 1;
}

int ref_ref_inc(int && a) {
	return ++a;
}

int main() {

	int a = 10;
	int& a_ref = a;
	int const& a_const_ref = 10;
	int&& a_ref_ref = 10;

	cout << bind(plain_inc, a)() << endl;
	cout << bind(plain_inc, a_ref)() << endl;
	cout << bind(plain_inc, a_const_ref)() << endl;
	cout << bind(plain_inc, a_ref_ref)() << endl << endl;

	cout << bind(ref_inc, a)() << endl;
	cout << bind(ref_inc, a_ref)() << endl;
	//cout << bind(ref_inc, a_const_ref)() << endl;
	cout << bind(ref_inc, a_ref_ref)() << endl << endl;

	cout << bind(cosnt_ref_inc, a)() << endl;
	cout << bind(cosnt_ref_inc, a_ref)() << endl;
	cout << bind(cosnt_ref_inc, a_const_ref)() << endl;
	cout << bind(cosnt_ref_inc, a_ref_ref)() << endl << endl;

	cout << bind(ref_inc, _1)(a) << endl;
	cout << bind(ref_inc, _1)(a_ref) << endl;
	//cout << bind(ref_inc, a_const_ref)() << endl;
	cout << bind(ref_inc, _1)(a_ref_ref) << endl;


	return 0;
}