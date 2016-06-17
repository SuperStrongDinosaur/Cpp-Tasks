#include "big_integer.h"
#include <algorithm>

big_integer::big_integer()
{
	data = std::vector<uint32_t>(0);
	sign = 0;
}

big_integer::big_integer(big_integer const &other)
{
	this->data = other.data;
	this->sign = other.sign;
}

big_integer::big_integer(int a)
{
	if (a == 0) {
		*this = big_integer();
	}
	else {
		data = std::vector<uint32_t>(1);
		if (a > 0) {
			sign = 1;
			data[0] = a;
		}
		else {
			sign = -1;
			data[0] = -a;
		}
	}
}

big_integer::big_integer(std::string const & str)
{
	size_t start = 0;
	*this = big_integer(0);
	int8_t sign = 1;
	if (str[0] == '-') {
		sign = -1;
		start = 1;
	}
	for (size_t i = start; i < str.size(); i++) {
		*this = this->mul(10);
		*this += (str[i] - '0');
	}
	this->sign = sign;
	this->remove_leading_zeros();
}

big_integer::~big_integer() {}

big_integer & big_integer::operator=(big_integer const & other)
{
	this->data = other.data;
	this->sign = other.sign;

	return *this;
}

big_integer & big_integer::operator+=(big_integer const & rhs)
{
	if (rhs.sign == 0) {
		return *this;
	}
	if (this->sign == 0) {
		*this = rhs;
		return *this;
	}
	if (this->sign == rhs.sign) {
		auto a = this->data;
		auto b = rhs.data;
		big_integer tmp = add_abs(a, b);
		this->data = tmp.data;
	}
	else {
		big_integer tmp = sub_abs(this->data, rhs.data);
		this->sign *= tmp.sign;
		this->data = tmp.data;
	}
	return *this;
}

big_integer & big_integer::operator-=(big_integer const & rhs)
{
	if (rhs.sign == 0) {
		return *this;
	}
	if (this->sign == 0) {
		*this = rhs;
		this->sign *= -1;
		return *this;
	}
	if (this->sign == rhs.sign) {
		big_integer tmp = sub_abs(this->data, rhs.data);
		this->sign *= tmp.sign;
		this->data = tmp.data;
	}
	else {
		this->data = add_abs(this->data, rhs.data).data;
	}
	return *this;
}

big_integer big_integer::mul(uint32_t const& rhs) const
{
	uint64_t r = 0;
	big_integer ans;
	ans.sign = this->sign;
	for (size_t i = 0; i < this->data.size(); i++) {
		r = static_cast<uint64_t>(this->data[i]) * rhs + r;
		ans.data.push_back(r % BASE);
		r /= BASE;
	}
	if (r) {
		ans.data.push_back(static_cast<uint32_t>(r));
	}
	return ans;
}

big_integer & big_integer::operator*=(big_integer const & t)
{
	int8_t sign = t.sign * this->sign;
	this->sign *= this->sign;
	big_integer rhs = t;
	rhs.sign *= rhs.sign;
	big_integer tmp = 0;
	for (size_t i = rhs.data.size(); i > 0; i--) {
		tmp <<= 32;
		uint32_t j = rhs.data[i - 1];
		tmp += this->mul(j);
	}
	this->data = tmp.data;
	this->sign = sign;
	return *this;
}

big_integer & big_integer::operator/=(big_integer const & rhs)
{
	if (compare_abs(this->data, rhs.data).sign < 0) {
		*this = big_integer(0);
		return *this;
	}
	if (rhs.data.size() == 1) {
		this->div_mod(rhs.data.back());
		this->sign *= rhs.sign;
		return *this;
	}
	uint32_t ratio = rhs.norm_ratio();
	big_integer A = this->mul(ratio);
	big_integer B = rhs.mul(ratio);
	int8_t sign = A.sign * B.sign;
	A.sign = 1;
	B.sign = 1;
	size_t n = B.data.size();
	size_t m = A.data.size() - n;
	B <<= static_cast<int>(m * 32);
	std::vector<uint32_t> q(m);
	if (compare_abs(A.data, B.data).sign >= 0) {
		q.push_back(1);
		A -= B;
	}
	for (size_t i = m; i > 0; i--) {
		if (A.sign == 0) {
			break;
		}
		B >>= 32;
		size_t j = i - 1;
		uint64_t q_tmp = (A.data[n + j] * BASE + A.data[n + j - 1]) / B.data.back();
		q[j] = static_cast<uint32_t>(std::min(q_tmp, BASE - 1));
		A -= B.mul(q[j]);
		while (A.sign < 0) {
			q[j]--;
			A += B;
		}
	}
	this->data = q;
	this->sign = sign;
	return *this;
}

big_integer &big_integer::operator%=(big_integer const &rhs)
{
	return *this -= (*this / rhs) * rhs;
}

big_integer & big_integer::operator&=(big_integer const & rhs)
{
	big_integer tmp(rhs);
	if (tmp.data.size() > this->data.size()) {
		std::swap(*this, tmp);
	}
	while (this->data.size() > tmp.data.size()) {
		tmp.data.push_back(0);
	}

	this->to_twos_complement();
	tmp.to_twos_complement();

	for (size_t i = 0; i < std::min(tmp.data.size(), this->data.size()); i++) {
		this->data[i] &= tmp.data[i];
	}

	this->from_twos_complement();
	this->remove_leading_zeros();
	return *this;
}

big_integer & big_integer::operator|=(big_integer const & rhs)
{
	big_integer tmp(rhs);
	if (tmp.data.size() > this->data.size()) {
		std::swap(*this, tmp);
	}
	while (this->data.size() > tmp.data.size()) {
		tmp.data.push_back(0);
	}

	this->to_twos_complement();
	tmp.to_twos_complement();

	for (size_t i = 0; i < std::min(tmp.data.size(), this->data.size()); i++) {
		this->data[i] |= tmp.data[i];
	}

	this->from_twos_complement();
	this->remove_leading_zeros();
	return *this;
}

big_integer & big_integer::operator^=(big_integer const & rhs)
{
	big_integer tmp(rhs);
	if (tmp.data.size() > this->data.size()) {
		std::swap(*this, tmp);
	}
	while (this->data.size() > tmp.data.size()) {
		tmp.data.push_back(0);
	}

	this->to_twos_complement();
	tmp.to_twos_complement();

	for (size_t i = 0; i < std::min(tmp.data.size(), this->data.size()); i++) {
		this->data[i] ^= tmp.data[i];
	}

	this->from_twos_complement();
	this->remove_leading_zeros();
	return *this;
}

big_integer & big_integer::operator<<=(int rhs)
{
	if (rhs == 0 || sign == 0) {
		return *this;
	}
	if (rhs < 0) return (*this >>= rhs);
	uint32_t digits = static_cast<uint32_t>(rhs) / 32;
	uint32_t mod = static_cast<uint32_t>(rhs) % 32;
	std::vector<uint32_t> tmp;

	this->to_twos_complement();

	if (mod) {
		tmp.push_back(0);
		for (size_t i = 0; i < data.size(); i++) {
			tmp.back() |= (data[i] << mod);
			tmp.push_back(data[i] >> (static_cast<uint32_t>(32) - mod));
		}
	}
	else {
		tmp = this->data;
	}

	this->data = std::vector<uint32_t>();
	for (size_t i = 0; i < digits; i++) {
		this->data.push_back(0);
	}
	for (auto i : tmp) {
		this->data.push_back(i);
	}

	this->from_twos_complement();
	this->remove_leading_zeros();
	return *this;
}

big_integer & big_integer::operator>>=(int rhs)
{
	if (rhs == 0 || sign == 0) {
		return *this;
	}
	if (rhs < 0) return (*this >>= rhs);
	uint32_t digits = static_cast<uint32_t>(rhs) / 32;
	uint32_t mod = static_cast<uint32_t>(rhs) % 32;
	std::vector<uint32_t> tmp;

	this->to_twos_complement();

	if (mod) {
		for (size_t i = 0; i < data.size(); i++) {
			if (!tmp.empty()) {
				tmp.back() |= (data[i] << (static_cast<uint32_t>(32) - mod));
			}
			if (i == data.size() - 1 && this->sign < 0) {
				auto a = static_cast<int32_t>(data[i]);
				a >>= mod;
				tmp.push_back(static_cast<uint32_t>(a));
			}
			else {
				tmp.push_back(data[i] >> mod);
			}
		}
	}
	else {
		tmp = this->data;
	}

	this->data = std::vector<uint32_t>();
	for (size_t i = digits; i < tmp.size(); i++) {
		this->data.push_back(tmp[i]);
	}

	this->from_twos_complement();
	this->remove_leading_zeros();
	return *this;
}

big_integer big_integer::operator+() const
{
	return *this;
}

big_integer big_integer::operator-() const
{
	big_integer tmp(*this);
	tmp.sign *= -1;
	return tmp;
}

big_integer big_integer::operator~() const
{
	big_integer tmp(*this);
	tmp += 1;
	tmp.sign = !tmp.sign;
	return tmp;
}

big_integer &big_integer::operator++()
{
	this->operator+=(1);
	return *this;
}

big_integer big_integer::operator++(int)
{
	big_integer r = *this;
	++*this;
	return r;
}

big_integer &big_integer::operator--()
{
	this->operator-=(1);
	return *this;
}

big_integer big_integer::operator--(int)
{
	big_integer r = *this;
	--*this;
	return r;
}

inline void big_integer::remove_leading_zeros()
{
	while (!this->data.empty() && !this->data.back()) {
		this->data.pop_back();
	}
	if (this->data.empty()) {
		this->sign = 0;
	}
}

big_integer big_integer::add_abs(std::vector<uint32_t> const& a, std::vector<uint32_t> const& b)
{
	size_t max_size = std::max(a.size(), b.size());
	big_integer c;
	c.data.resize(max_size);
	c.sign = 1;
	uint64_t r = 0;
	for (size_t i = 0; i < max_size; i++) {
		r = (i < a.size() ? static_cast<uint64_t>(a[i]) : 0) + (i < b.size() ? static_cast<uint64_t>(b[i]) : 0) + r;
		c.data[i] = r % BASE;
		r /= BASE;
	}
	if (r) {
		c.data.push_back(static_cast<uint32_t>(r));
	}
	return c;
}

big_integer big_integer::sub_abs(std::vector<uint32_t> const& a, std::vector<uint32_t> const& b)
{
	big_integer c = compare_abs(a, b);
	if (c.sign < 0) {
		c.data = compare_abs(b, a).data;
	}
	return c;
}

big_integer big_integer::compare_abs(std::vector<uint32_t> const& a, std::vector<uint32_t> const& b)
{
	size_t max_size = std::max(a.size(), b.size());
	big_integer c;
	c.data.resize(max_size);
	int64_t r = 0;
	for (size_t i = 0; i < max_size; i++) {
		r = (i < a.size() ? static_cast<uint64_t>(a[i]) : 0) - (i < b.size() ? static_cast<uint64_t>(b[i]) : 0) + r;
		if (r < 0) {
			c.data[i] = static_cast<uint32_t>(r + BASE);
			r = -1;
		}
		else {
			c.data[i] = static_cast<uint32_t>(r);
			r = 0;
		}
	}
	if (r) {
		c.sign = -1;
	}
	else {
		c.sign = 1;
	}
	c.remove_leading_zeros();
	return c;
}

uint32_t big_integer::div_mod(uint32_t const& a)
{
	uint32_t r = 0;
	for (size_t j = this->data.size(); j > 0; j--) {
		size_t i = j - 1;
		uint64_t tmp = static_cast<uint64_t>(this->data[i]) + static_cast<uint64_t>(r) * BASE;
		this->data[i] = static_cast<uint32_t>(tmp / a);
		r = tmp % a;
	}

	remove_leading_zeros();
	return r;
}

uint32_t big_integer::norm_ratio() const
{
	if (sign == 0) {
		return 0;
	}
	uint32_t ratio = 1;
	while (data.back() * ratio < static_cast<uint32_t>(BASE / 2)) {
		ratio *= 2;
	}
	return ratio;
}

inline void big_integer::to_twos_complement()
{
	if (this->data.back() >> 31) {
		this->data.push_back(0);
	}
	if (this->sign < 0) {
		*this += 1;
		for (auto& i : this->data) {
			i = ~i;
		}
	}
}

inline void big_integer::from_twos_complement()
{
	if (this->sign == 0) {
		return;
	}
	if (this->data.back() >> 31) {
		this->sign = -1;
		//this->data.back() -= (1 << 31);
	}
	else {
		this->sign = 1;
	}
	if (this->sign < 0) {
		*this += 1;
		for (auto& i : this->data) {
			i = ~i;
		}
	}
}

big_integer operator+(big_integer a, big_integer const& b)
{
	return a += b;
}

big_integer operator-(big_integer a, big_integer const& b)
{
	return a -= b;
}

big_integer operator*(big_integer a, big_integer const& b)
{
	return a *= b;
}

big_integer operator/(big_integer a, big_integer const& b)
{
	return a /= b;
}

big_integer operator%(big_integer a, big_integer const& b)
{
	return a %= b;
}

big_integer operator&(big_integer a, big_integer const& b)
{
	return a &= b;
}

big_integer operator|(big_integer a, big_integer const& b)
{
	return a |= b;
}

big_integer operator^(big_integer a, big_integer const& b)
{
	return a ^= b;
}

big_integer operator<<(big_integer a, int b)
{
	return a <<= b;
}

big_integer operator >> (big_integer a, int b)
{
	return a >>= b;
}

bool operator==(big_integer const& a, big_integer const& b)
{
	return big_integer::compare_abs(a.data, b.data).sign == 0;
}

bool operator!=(big_integer const& a, big_integer const& b)
{
	return big_integer::compare_abs(a.data, b.data).sign != 0;
}

bool operator<(big_integer const& a, big_integer const& b)
{
	return big_integer::compare_abs(a.data, b.data).sign < 0;
}

bool operator>(big_integer const& a, big_integer const& b)
{
	return big_integer::compare_abs(a.data, b.data).sign > 0;
}

bool operator<=(big_integer const& a, big_integer const& b)
{
	return big_integer::compare_abs(a.data, b.data).sign <= 0;
}

bool operator>=(big_integer const& a, big_integer const& b)
{
	return big_integer::compare_abs(a.data, b.data).sign >= 0;
}

std::string to_string(big_integer const & a)
{
	if (a.sign == 0) {
		return "0";
	}

	big_integer tmp = a;
	std::string s;

	while (tmp.sign) {
		s = std::to_string(tmp.div_mod(10)) + s;
	}
	if (a.sign < 0) {
		s = '-' + s;
	}

	return s;
}

std::ostream& operator<<(std::ostream& s, big_integer const& a)
{
	return s << to_string(a);
}