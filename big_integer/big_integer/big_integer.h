#ifndef BIGINT_BIG_INTEGER_H
#define BIGINT_BIG_INTEGER_H

#include <string>
#include <vector>
#include "my_vec.h"

class big_integer {
public:
	big_integer() : data(1), sign(true) {}
	big_integer(int a);
	explicit big_integer(std::string const &str);
	big_integer(big_integer const &other) : data(other.data), sign(other.sign) {}
	~big_integer();

	big_integer &operator=(big_integer const &other);
	big_integer &operator+=(big_integer const &rhs);
	big_integer &operator-=(big_integer const &rhs);
	big_integer &operator*=(big_integer const &rhs);
	big_integer &operator/=(big_integer const &rhs);
	big_integer &operator%=(big_integer const &rhs);
	big_integer &operator&=(big_integer const &rhs);
	big_integer &operator|=(big_integer const &rhs);
	big_integer &operator^=(big_integer const &rhs);
	big_integer &operator<<=(int rhs);
	big_integer &operator>>=(int rhs);

	big_integer operator+() const;
	big_integer operator-() const;
	big_integer operator~() const;
	big_integer &operator++();
	big_integer operator++(int);
	big_integer &operator--();
	big_integer operator--(int);

	friend bool operator==(big_integer const &a, big_integer const &b);
	friend bool operator!=(big_integer const &a, big_integer const &b);
	friend bool operator<(big_integer const &a, big_integer const &b);
	friend bool operator>(big_integer const &a, big_integer const &b);
	friend bool operator<=(big_integer const &a, big_integer const &b);
	friend bool operator>=(big_integer const &a, big_integer const &b);

	friend std::string to_string(big_integer const &a);

private:
	my_vec data;
	bool sign;
	const static uint64_t BASE = static_cast<uint64_t>(UINT32_MAX) + 1;
	const int basepow = 32;

	inline void flip_bytes(big_integer &a);
	inline void make_zero();
	inline void remove_leading_zeros();
	big_integer &abstract_operation(big_integer &a, big_integer b, uint32_t(*logicFunc)(uint32_t, uint32_t), bool(*check)(bool, bool));
	big_integer mul(big_integer const &b, uint32_t x);
	int big_integer::compare_abs(big_integer const& a, big_integer const& b);
};

big_integer operator+(big_integer a, big_integer const &b);
big_integer operator-(big_integer a, big_integer const &b);
big_integer operator*(big_integer a, big_integer const &b);
big_integer operator/(big_integer a, big_integer const &b);
big_integer operator%(big_integer a, big_integer const &b);
big_integer operator&(big_integer a, big_integer const &b);
big_integer operator|(big_integer a, big_integer const &b);
big_integer operator^(big_integer a, big_integer const &b);
big_integer operator<<(big_integer a, int b);
big_integer operator>>(big_integer a, int b);
bool operator==(big_integer const &a, big_integer const &b);
bool operator!=(big_integer const &a, big_integer const &b);
bool operator<(big_integer const &a, big_integer const &b);
bool operator>(big_integer const &a, big_integer const &b);
bool operator<=(big_integer const &a, big_integer const &b);
bool operator>=(big_integer const &a, big_integer const &b);

std::string to_string(big_integer const &a);
std::ostream &operator<<(std::ostream &s, big_integer const &a);

#endif //BIGINT_BIG_INTEGER_H