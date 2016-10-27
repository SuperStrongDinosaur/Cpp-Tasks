#include "my_vec.h"

my_vec::my_vec(size_t size) : sz(size), data(new saved_data) {
	if (size <= 1)
		data->val = 0;
	else
		data->vals = new std::vector<uint32_t>(size);
}

my_vec::my_vec() : my_vec(0) {}

my_vec::my_vec(const my_vec& other) {
	data = other.data;
	sz = other.sz;
}

my_vec & my_vec::operator=(const my_vec & other) {
	data = other.data;
	sz = other.sz;
	return *this;
}

my_vec::~my_vec() {
	if (sz > 1 && data.use_count() == 1) {
		delete data->vals;
		data->vals = nullptr;
	}
}

void my_vec::correct() {
	size_t temp_sz = data->vals->size();
	if (sz > 1 && data->vals->size() == 1) {
		uint32_t curr = (*data->vals)[0];
		delete data->vals;
		data->val = curr;
	}
	sz = temp_sz;
}

void my_vec::make_own() {
	if (data.use_count() > 1) {
		auto prev = data;
		data = std::make_shared<saved_data>();
		if (sz <= 1)
			data->val = prev->val;
		else if (sz && prev->vals != nullptr)
			data->vals = new std::vector<uint32_t>(*(prev->vals));
	}
}

void my_vec::push_back(uint32_t val) {
	make_own();
	if (sz == 0)
		data->val = val;
	else if (sz == 1) {
		std::vector<uint32_t>* curr = new std::vector<uint32_t>();
		curr->push_back(data->val);
		curr->push_back(val);
		data->vals = curr;
	}
	else
		data->vals->push_back(val);
	sz++;
}

void my_vec::pop_back() {
	make_own();
	if (sz > 1) {
		data->vals->pop_back();
		correct();
	}
	else if (sz == 1)
		sz--;
}

uint32_t my_vec::back() const {
	if (sz == 1)
		return data->val;
	return data->vals->back();
}

size_t my_vec::size() const {
	return sz;
}

void my_vec::resize(size_t len) {
	make_own();
	if (sz <= 1) {
		std::vector<uint32_t>* tmp = new std::vector<uint32_t>;
		if (sz == 1)
			tmp->push_back(data->val);
		data->vals = tmp;
	}
	data->vals->resize(len);
	correct();
}

uint32_t my_vec::operator[](size_t index) const {
	if (sz == 1)
		return data->val;
	return (*data->vals)[index];

}

uint32_t& my_vec::operator[](size_t index) {
	make_own();
	if (sz == 1)
		return data->val;
	return (*data->vals)[index];
}

void my_vec::reverse() {
	make_own();
	if (sz > 1)
		std::reverse(data->vals->begin(), data->vals->end());
}