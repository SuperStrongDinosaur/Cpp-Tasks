#ifndef MY_VEC_H
#define MY_VEC_H

#include <vector>
#include <memory>

using namespace std;

struct my_vec {
	my_vec();
	my_vec(size_t sz);
	my_vec(const my_vec& other);

	my_vec& operator=(const my_vec& other);
	~my_vec();

	void push_back(uint32_t val);
	void pop_back();

	uint32_t back() const;
	size_t size() const;
	void resize(const size_t size);

	uint32_t operator[] (size_t index) const;
	uint32_t& operator[] (size_t index);
	void reverse();

private:
	uint32_t val;
	shared_ptr<vector<uint32_t>> vals;
	size_t sz;

	void make_own();
};

#endif