#include "LFUcache.h"
#include <iostream>
#include <string>
#include <fstream>
#include <vector>

using namespace std;

template <class K, class V>
void do_testLFU(int lol) {
	string file = "tests/LFUcachetest";
	file += lol + '0';
	file += ".txt";
	ifstream in;
	in.open(file);

	int n, m;
	in >> n >> m; 
	LFUcache<K, V> *b = new LFUcache<K, V>(n);

	cout << "testLFU: " << lol << endl;

	for (int i = 0; i < m; i++) {
		string oper;
		in >> oper;

		if (oper == "get") {
			K key;
			in >> key;

			try {
				V value = b->Get(key);
				V ans_value;
				in >> ans_value;
				if (value != ans_value)
					cout << "Error in LFU cache" << endl;
				else
					cout << "Ok" << endl;
			} catch (const CacheNotFoundException &e) {
				string s;
				in >> s;
				if (s != "exception")
					cout << "Error in LFU cache" << endl;
				else
					cout << "Ok" << endl;
			}
		} else {
			K key;
			V value;
			in >> key >> value;
			b->Put(key, value);
		}
	}
}
