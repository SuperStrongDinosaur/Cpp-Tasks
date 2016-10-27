#ifndef MY_VEC_H
#define MY_VEC_H

#include <vector>
#include <memory>

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
	void resize(const size_t sz);

	uint32_t operator[] (size_t index) const;
	uint32_t& operator[] (size_t index);

	void reverse();

private:

	union saved_data {
		uint32_t val;
		std::vector<uint32_t>* vals;
	};

	std::shared_ptr<saved_data> data;
	size_t sz;
	void correct();
	void make_own();
};


#endif