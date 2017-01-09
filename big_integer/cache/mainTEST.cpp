#include "FIFOtest.cpp"
#include "LFUtest.cpp"
#include "LRUtest.cpp"

const int cnt_of_LRUtests = 3;
const int cnt_of_FIFOtests = 3;
const int cnt_of_LFUtests = 3;

int main() {
	for(int i = 0; i < cnt_of_LRUtests; i++)
		do_testLRU<int, int>(i);

	for (int i = 0; i < cnt_of_FIFOtests; i++)
		do_testFIFO<int, int>(i);

	for (int i = 0; i < cnt_of_LFUtests; i++)
		do_testLFU<int, int>(i);
	return 0;
}