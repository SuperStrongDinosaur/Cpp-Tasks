#include "LRUcache.h"
#include <iostream>
#include <string>
#include <fstream>
#include <vector>

using namespace std;

template <class K, class V>
void do_testLRU(int lol) {
	string file = "tests/LRUcachetest";
	file += lol + '0';
	file += ".txt";
	ifstream in;
	in.open(file);

	int n, m;
	in >> n >> m; 
	LRUcache<K, V> *c = new LRUcache<K, V>(n);

	cout << "testLRU: " << lol << endl;

	for (int i = 0; i < m; i++) {
		string oper;
		in >> oper;

		if (oper == "get") {
			K key;
			in >> key;

			try {
				V value = c->Get(key);
				V ans_value;
				in >> ans_value;
				if (value != ans_value)
					cout << "Error in LRU cache" << endl;
				else
					cout << "Ok" << endl;
			} catch (const CacheNotFoundException &e) {
				string s;
				in >> s;
				if (s != "exception")
					cout << "Error in LRU cache" << endl;
				else
					cout << "Ok" << endl;
			}
		} else {
			K key;
			V value;
			in >> key >> value;
			c->Put(key, value);
		}
	}
}
