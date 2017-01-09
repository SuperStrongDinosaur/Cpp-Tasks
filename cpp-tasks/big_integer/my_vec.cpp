#include "my_vec.h"

my_vec::my_vec(size_t size) : sz(size) {
	if (size < 2) 
		val = 0;
	else 
		vals = make_shared<vector<uint32_t>>(size);
}

my_vec::my_vec() : my_vec(0) {}

my_vec::my_vec(const my_vec& other) : sz(other.sz) {
	if (sz < 2)
		val = other.val;
	else
		vals = other.vals;
}

my_vec & my_vec::operator=(const my_vec & other) {
	bool is_equal = true;
	if (sz != other.sz)
		is_equal = false;
	if (is_equal && sz > 1) {
		for (size_t i = 0; i != sz; i++)
			if ((*this)[i] != (*other.vals)[i])
				is_equal = false;
	}
	else if(is_equal && sz == 1 && val != other.val)
		is_equal = false;
	if (is_equal)
		return *this;
	if (other.sz > 1)
		vals = other.vals;
	else
	{
		val = other.val;
		if (sz > 1)
			vals.reset();
	}
	sz = other.sz;
	return *this;
}

my_vec::~my_vec() {}

void my_vec::make_own() {
	if (!vals.unique()) 
		if (sz > 1) {
			auto tmp = std::make_shared<std::vector<uint32_t>>(*vals);
			vals = tmp;
		}
}

void my_vec::push_back(uint32_t val1) {
	make_own();
	if (sz == 0)
		val = val1;
	else if (sz == 1) {
		vals = make_shared<vector<uint32_t>>();
		vals->push_back(val);
		vals->push_back(val1);
	}
	else
		vals->push_back(val1);
	sz++;
}

void my_vec::pop_back() {
	make_own();
	if (sz < 2)
		return;
	if (sz == 2) {
		val = (*vals)[0];
		vals.reset();
	}
	else
		(*vals).pop_back();
	sz--;
}

uint32_t my_vec::back() const {
	if (sz == 1)
		return val;
	return vals->back();
}

size_t my_vec::size() const {
	return sz;
}

void my_vec::resize(size_t size) {
	make_own();
	if (sz == size)
		return;
	else if (size == 0) {
		val = 0;
		vals.reset();
	}
	else if (sz < 2 && size > 1) {
		vals = std::make_shared<std::vector<unsigned>>(size);
		(*vals)[0] = val;
	}
	else if (sz > 1 && size < 2) {
		val = (*vals)[0];
		vals.reset();
	} else  
		(*vals).resize(size);
	sz = size;
}

uint32_t my_vec::operator[](size_t index) const {
	if (sz == 1)
		return val;
	return (*vals)[index];

}

uint32_t& my_vec::operator[](size_t index) {
	make_own();
	if (sz < 2)
		return val;
	return (*vals)[index];
}

void my_vec::reverse() {
	make_own();
	if (sz > 1)
		std::reverse((*vals).begin(), (*vals).end());
}
