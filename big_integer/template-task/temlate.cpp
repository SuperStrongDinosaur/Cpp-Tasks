#include <cassert>
#include <limits>
#include <iostream>

using namespace std;

struct unsigned_tag {};

struct signed_tag {};

template <bool T>
struct checked_traits {
	typedef signed_tag tag;
};

template <>
struct checked_traits<false> {
	typedef unsigned_tag tag;
};

template <typename T>
T check_add(T l, T r, unsigned_tag) {
	assert((r <= 0) || (numeric_limits<T>::max() - r >= l));
	return l + r;
}

template <typename T>
T check_add(T l, T r, signed_tag) {
	assert(((r < 0) && (numeric_limits<T>::min() - r > l))) || ((r <= 0) || (numeric_limits<T>::max() - r >= l));
	return l + r;
}

template <typename T>
T check_sub(T l, T r, unsigned_tag) {
	assert((r <= 0) || (l >= numeric_limits<T>::min() + r));
	return l - r;
}

template <typename T>
T check_sub(T l, T r, signed_tag) {
	assert(((r < 0) && (l > numeric_limits<T>::max() + r))) || ((r <= 0) || (l >= numeric_limits<T>::min() + r));
	return l - r;
}

template <typename T>
T check_mul(T l, T r) {
	if (r != 0 && l != 0)
		assert(((l * r) / r) == l && (l * r / l) == r);
	return l * r;
}

template <typename T>
T check_div(T l, T r, signed_tag) {
	assert(r != 0);
	assert((l != numeric_limits<T>::min()) || (r != -1));
	return l / r;
}

template <typename T>
T check_unary(T num, signed_tag) {
	assert(num != numeric_limits<T>::min());
	return -num;
}

template <typename T>
T check_div(T l, T r, unsigned_tag) {
	assert(r != 0);
	return l / r;
}

template <typename T>
T check_unary(T num, unsigned_tag) {
	assert(num == 0);
	return -num;
}

template <typename T>
class my_num {
	T num;
public:

	my_num(T a) : num(a) {}

	my_num operator+=(my_num& other) {
		num = check_add(num, other.num, typename checked_traits<numeric_limits<T>::is_signed>::tag());
		return *this;
	}

	my_num operator-=(my_num other) {
		num = check_sub(num, other.num, typename checked_traits<numeric_limits<T>::is_signed>::tag());
		return *this;
	}

	my_num operator*=(my_num other) {
		num = check_mul(num, other.num);
		return *this;
	}

	my_num operator/=(my_num other) {
		num = check_div(num, other.num, typename checked_traits<numeric_limits<T>::is_signed>::tag());
		return *this;
	}

	my_num operator-() {
		num = check_unary(num, typename checked_traits<numeric_limits<T>::is_signed>::tag());
		return *this;
	}

	friend ostream& operator<<(ostream& os, const my_num& obj) {
		return os << obj.num;
	}
};

template <typename T>
my_num<T> operator+(my_num<T> a, my_num<T> b) {
	return a += b;
}

template <typename T>
my_num<T> operator-(my_num<T> a, my_num<T> b) {
	return a -= b;
}

template <typename T>
my_num<T> operator*(my_num<T> a, my_num<T> b) {
	return a *= b;
}

template <typename T>
my_num<T> operator/(my_num<T> a, my_num<T> b) {
	return a /= b;
}

int main() {
	return 0;
}