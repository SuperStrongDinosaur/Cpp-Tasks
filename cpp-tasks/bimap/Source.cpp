#include <iostream>
#include <vector>
#include "bimap.h"

using namespace std;

int main() {
	bimap a;
	int sch = 0, genered;
	vector < int > data;
	for (int i = 0; i < 50; i++) {
		genered = rand();
		data.push_back(genered);
		a.insert(genered, rand());
	}
	int ind = 0;
	bimap::left_iterator itt;
	while (a.begin_left() != a.end_left()) {
		std::cout << *a.begin_left() <<  " " << *a.end_left() << endl;
		itt = a.find_left(data[ind]);
		a.erase(itt);
		++ind;
		++sch;
	}

	cout << sch << endl;
	return 0;
}