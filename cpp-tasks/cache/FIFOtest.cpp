#include "FIFOcache.h"
#include <iostream>
#include <string>
#include <fstream>
#include <vector>

using namespace std;

template <class K, class V>
void do_testFIFO(int lol) {
	string file = "tests/FIFOcachetest";
	file += lol + '0';
	file += ".txt";
	ifstream in;
	in.open(file);

	int n, m;
	in >> n >> m;
	FIFOcache<K, V> *a = new FIFOcache<K, V>(n);

	cout << "testFIFO: " << lol << endl;

	for (int i = 0; i < m; i++) {
		string oper;
		in >> oper;

		if (oper == "get") {
			K key;
			in >> key;
			try {
				V value = a->Get(key);
				V ans_value;
				in >> ans_value;
				if (value != ans_value)
					cout << "Error in FIFO cache" << endl;
				else
					cout << "Ok" << endl;
			} catch (const CacheNotFoundException &e) {
				string s;
				in >> s;
				if (s != "exception")
					cout << "Error in FIFO cache" << endl;
				else
					cout << "Ok" << endl;
			}
		} else {
			K key;
			V value;
			in >> key >> value;
			a->Put(key, value);
		}
	}
}
