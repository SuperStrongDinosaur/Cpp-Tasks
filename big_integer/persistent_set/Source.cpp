#include <iostream>
#include "pers_set.h"
#include <vector>
#include <algorithm>    // std::random_shuffle
#include <string>
using namespace std;

void big_data_test(int amount) {
	persistent_set my_set;
	vector<int> params;
	for (int i = 0; i < amount; ++i) {
		params.push_back(i);
	}
	random_shuffle(params.begin(), params.end());
	for (int i = 0; i < amount; ++i) {
		my_set.insert(params[i]);
	}
	while (my_set.begin() != my_set.end()) {
		my_set.erase(my_set.begin());
	}
	return;
}
void copy_test(int amount) {
	int kol = 0;
	persistent_set copy_set;
	{
		persistent_set my_set;
		vector<int> params;
		for (int i = 0; i < amount; ++i) {
			params.push_back(i);
		}
		random_shuffle(params.begin(), params.end());
		for (int i = 0; i < amount; ++i) {
			my_set.insert(params[i]);
		}
		copy_set = my_set;
	}
	while (copy_set.begin() != copy_set.end()) {
		copy_set.erase(copy_set.begin());
		kol++;
	}
	if (kol != amount) cout << "error copy_test " << kol << " / " << amount << endl;
}
void insert_erase_test() {
	persistent_set my_set;
	my_set.insert(40);
	my_set.insert(20);
	my_set.insert(60);
	my_set.insert(10);
	my_set.insert(30);
	my_set.insert(50);
	my_set.insert(70);
	my_set.insert(45);
	my_set.insert(55);
	my_set.insert(43);
	my_set.insert(47);
	my_set.insert(46);
	my_set.insert(48);
	my_set.insert(49);

	my_set.erase(my_set.find(50));
	{
		persistent_set copy_set(my_set);
		int kol = 0;
		while (copy_set.begin() != copy_set.end()) {
			if (*copy_set.begin() == 50) cout << "error insert_erase_test\n";
			copy_set.erase(copy_set.begin());
			kol++;
		}
		if (kol != 13) cout << "error insert_erase_test\n";
	}

	my_set.erase(my_set.find(49));
	{
		persistent_set copy_set(my_set);
		int kol = 0;
		while (copy_set.begin() != copy_set.end()) {
			if (*copy_set.begin() == 49) cout << "error insert_erase_test\n";
			copy_set.erase(copy_set.begin());
			kol++;
		}
		if (kol != 12) cout << "error insert_erase_test\n";
	}

	my_set.erase(my_set.find(55));
	{
		persistent_set copy_set(my_set);
		int kol = 0;
		while (copy_set.begin() != copy_set.end()) {
			if (*copy_set.begin() == 55) cout << "error insert_erase_test\n";
			copy_set.erase(copy_set.begin());
			kol++;
		}
		if (kol != 11) cout << "error insert_erase_test\n";
	}

	my_set.erase(my_set.find(40));
	{
		persistent_set copy_set(my_set);
		int kol = 0;
		while (copy_set.begin() != copy_set.end()) {
			if (*copy_set.begin() == 40) cout << "error insert_erase_test\n";
			copy_set.erase(copy_set.begin());
			kol++;
		}
		if (kol != 10) cout << "error insert_erase_test\n";
	}
	while (my_set.begin() != my_set.end()) {
		cout << *my_set.begin() << ' ';
		my_set.erase(my_set.begin());
	}
	cout << "\n10 20 30 43 45 46 47 48 60 70 \n";
}
void iterator_test() {
	persistent_set my_set;
	my_set.insert(40);
	my_set.insert(20);
	my_set.insert(60);
	my_set.insert(10);
	my_set.insert(30);
	my_set.insert(50);
	my_set.insert(70);
	my_set.insert(45);
	my_set.insert(55);
	my_set.insert(43);
	my_set.insert(47);
	my_set.insert(46);
	my_set.insert(48);
	my_set.insert(49);

	//    cout << *my_set.find(70) <<endl;
	//    cout << *(++my_set.find(70)) <<endl;
	//
	//    cout << ((*++my_set.find(70) == *my_set.end()) ? "TRUE" : "FALSE") << endl;
	//
	for (persistent_set::iterator it = my_set.begin(); it != my_set.end(); it++) {
		cout << *it << ' ';
	}
	cout << endl;
	for (persistent_set::iterator it = my_set.begin(); it != my_set.end(); ++it) {
		cout << *it << ' ';
	}
	cout << "\n10 20 30 40 43 45 46 47 48 49 50 55 60 70 \n\n";
	for (persistent_set::iterator it = --my_set.end(); it != my_set.begin(); --it) {
		cout << *it << ' ';
	}
	cout << *my_set.begin() << endl;
	for (persistent_set::iterator it = --my_set.end(); it != my_set.begin(); it--) {
		cout << *it << ' ';
	}
	cout << *my_set.begin() << endl;
	cout << "70 60 55 50 49 48 47 46 45 43 40 30 20 10\n";

}

int main() {


	string a;

	cout << "Start big_data_test \n";
	big_data_test(1 * 1e5);
	cout << "Complete big_data_test \n\n";

	cout << "Start big_data_stress_test \n";
	for (int i = 0; i < 10; ++i) big_data_test(1 * 1e4);
	cout << "Complete big_data_stress_test \n\n";

	cout << "Start copy_test \n";
	copy_test(1e5);
	cout << "Complete copy_test \n\n";

	cout << "Start copy_stress_test \n";
	for (int i = 0; i < 10; ++i) copy_test(1e4);
	cout << "Complete copy_stress_test \n\n";

	cout << "Start insert_erase_test \n";
	insert_erase_test();
	cout << "Complete insert_erase_test \n\n";

	cout << "Start iterator_test \n";
	iterator_test();
	cout << "Complete iterator_test \n\n";


	//cin >> a;
	return 0;
}