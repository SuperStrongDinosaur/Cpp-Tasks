#include "big_integer.h"
#include <algorithm>

big_integer::big_integer() {
	data = std::vector<uint32_t>(1);
	sign = true;
}

big_integer::big_integer(int a) {
	if (a == 0) 
		*this = big_integer();
	else {
		data = std::vector<uint32_t>(1);
		if (a > 0) {
			sign = true;
			data[0] = a;
		}
		else {
			sign = false;
			data[0] = -a;
		}
	}
}

big_integer::big_integer(std::string const &str) {
	size_t start = 0;
	*this = big_integer(0);
	for (size_t i = (str[0] == '-' ? 1 : 0); i < str.size(); i++) {
		*this *= 10;
		*this += (str[i] - '0');
	}
	if (str[0] == '-') 
		*this = -*this;
}

big_integer &big_integer::operator=(big_integer const &other) {
	this->data = other.data;
	this->sign = other.sign;
	return *this;
}

big_integer &big_integer::operator+=(big_integer const &rhs) {
	big_integer t = rhs;
	if (sign == t.sign) {
		uint32_t carry = 0;
		int64_t cur = 0;
		for (size_t i = 0; i < std::max(data.size(), t.data.size()) || carry; i++) {
			if (i == data.size())
				data.push_back(0);
			cur = static_cast<int64_t>(data[i]) + static_cast<int64_t>(carry) + static_cast<int64_t>((i < t.data.size() ? t.data[i] : 0));
			data[i] = static_cast<uint32_t>(cur % BASE);
			carry = static_cast<uint32_t>(cur / BASE);
		}
	}
	else {
		bool saveSign = sign;
		sign = true;
		t.sign = true;
		bool newSign;
		if (*this > t) 
			newSign = saveSign;
		else 
			newSign = !saveSign;
		*this -= t;
		sign = newSign;
	}
	this->make_zero();
	return *this;
}

big_integer &big_integer::operator-=(big_integer const &rhs) {
	big_integer a = *this;
	a.sign = true;
	big_integer b = rhs;
	b.sign = true;
	if (this->sign == rhs.sign) {
		if (a < b)
			std::swap(a, b);
		int64_t carry = 0, cur = 0;
		for (size_t i = 0; i < b.data.size() || carry; ++i) {
			cur = static_cast<int64_t>(a.data[i]);
			cur -= carry + static_cast<int64_t>(i < b.data.size() ? b.data[i] : 0);
			carry = (cur < 0);
			if (carry) cur += BASE;
			a.data[i] = static_cast<uint32_t>(cur);
		}
		a.remove_leading_zeros();
		if (compare_abs(*this, rhs) > 0) 
			sign = this->sign;
		else 
			sign = !this->sign;
	}
	else 
		a += b;
	this->data = a.data;
	this->make_zero();
	return *this;
}

big_integer & big_integer::operator*=(big_integer const &t)
{
	bool sign = !t.sign ^ this->sign;
	this->sign ^= !this->sign;
	big_integer rhs = t;
	rhs.sign ^= !rhs.sign;
	big_integer tmp = 0;
	for (size_t i = rhs.data.size(); i > 0; i--) {
		tmp <<= basepow;
		tmp += mul(*this, rhs.data[i - 1]);
	}
	this->data = tmp.data;
	this->sign = sign;
	return *this;
}

big_integer & big_integer::operator/=(big_integer const & rhs) {
	if (compare_abs(*this, rhs) < 0) {
		*this = big_integer(0);
		return *this;
	}
	if (rhs.data.size() == 1) {
		uint32_t r = 0, a = rhs.data.back();
		for (size_t j = this->data.size(); j > 0; j--) {
			uint64_t tmp = static_cast<uint64_t>(this->data[j - 1]) + static_cast<uint64_t>(r) * BASE;
			this->data[j - 1] = static_cast<uint32_t>(tmp / a);
			r = tmp % a;
		}
		remove_leading_zeros();
		this->sign ^= !rhs.sign;
		return *this;
	}
	uint32_t normalize;
	if (rhs.data[0] == 0 && rhs.data.size() == 1)
		normalize = 0;
	else {
		normalize = 1;
		while (rhs.data.back() * normalize < static_cast<uint32_t>(BASE / 2))
			normalize *= 2;
	}
	big_integer A = mul(*this, normalize);
	big_integer B = mul(rhs, normalize);
	bool sign = A.sign ^ !B.sign;
	A.sign = true;
	B.sign = true;
	size_t n = B.data.size();
	size_t m = A.data.size() - n;
	B <<= static_cast<int>(m * basepow);
	std::vector<uint32_t> q(m);
	if (compare_abs(A, B) >= 0) {
		q.push_back(1);
		A -= B;
	}
	for (size_t i = m - 1; i >= 0; i--) {
		if (A.data[0] == 0 && A.data.size() == 1) 
			break;
		B >>= basepow;
		uint64_t q_tmp = (A.data[n + i] * BASE + A.data[n + i - 1]) / B.data.back();
		q[i] = static_cast<uint32_t>(std::min(q_tmp, BASE - static_cast<uint64_t>(1)));
		A -= mul(B, q[i]);
		while (!A.sign) {
			q[i]--;
			A += B;
		}
	}
	this->data = q;
	this->sign = sign;
	return *this;
}

big_integer &big_integer::operator%=(big_integer const &rhs) {  return *this -= (*this / rhs) * rhs; }
big_integer &big_integer::operator&=(big_integer const &rhs) { return abstractLogicOperation(*this, big_integer(rhs), [](uint32_t a, uint32_t b) -> uint32_t { return a & b; }, [](bool x, bool y) -> bool { return x & y; }); }
big_integer &big_integer::operator|=(big_integer const &rhs) { return abstractLogicOperation(*this, big_integer(rhs), [](uint32_t a, uint32_t b) -> uint32_t { return a | b; }, [](bool x, bool y) -> bool { return x | y; }); }
big_integer &big_integer::operator^=(big_integer const &rhs) { return abstractLogicOperation(*this, big_integer(rhs), [](uint32_t a, uint32_t b) -> uint32_t { return a ^ b; }, [](bool x, bool y) -> bool { return x ^ y; }); }

big_integer &big_integer::operator<<=(int rhs)
{
	std::reverse(this->data.begin(), this->data.end());
	for (int i = 0; i < rhs / basepow; i++) 
		this->data.push_back(0);
	std::reverse(this->data.begin(), this->data.end());
	for (int i = 0; i < rhs % basepow; i++)
		*this *= 2;
	this->remove_leading_zeros();
	return *this;
}

big_integer &big_integer::operator>>=(int rhs) {
	std::reverse(this->data.begin(), this->data.end());
	bool thissigne = sign;
	for (int i = 0; i < rhs / basepow; i++) 
		this->data.pop_back();
	std::reverse(this->data.begin(), this->data.end());
	for (int i = 0; i < rhs % basepow; i++) 
		*this /= 2;
	if (!thissigne) 
		*this -= 1;
	this->remove_leading_zeros();
	return *this;
}

big_integer big_integer::operator+() const { return *this; }
big_integer big_integer::operator-() const {
	big_integer a = *this;
	if (a != 0)	a.sign = !sign;
	return a;
}

big_integer big_integer::operator~() const { return (-*this) - 1; }
big_integer &big_integer::operator++() { return *this += 1; }
big_integer big_integer::operator++(int) { return *this += 1; }
big_integer &big_integer::operator--() { return *this -= 1; }
big_integer big_integer::operator--(int) { return *this -= 1; }

big_integer operator+(big_integer a, big_integer const &b) { return a += b; }
big_integer operator-(big_integer a, big_integer const &b) { return a -= b; }
big_integer operator*(big_integer a, big_integer const &b) { return a *= b; }
big_integer operator/(big_integer a, big_integer const &b) { return a /= b; }
big_integer operator%(big_integer a, big_integer const &b) { return a %= b; }
big_integer operator&(big_integer a, big_integer const &b) { return a &= b; }
big_integer operator|(big_integer a, big_integer const &b) { return a |= b; }
big_integer operator^(big_integer a, big_integer const &b) { return a ^= b; }
big_integer operator<<(big_integer a, int b) { return a <<= b; }
big_integer operator>>(big_integer a, int b) { return a >>= b; }

bool operator==(big_integer const &a, big_integer const &b) {
	if (a.sign == b.sign) 
		if (a.data.size() == b.data.size()) {
			for (size_t i = 0; i < a.data.size(); i++)
				if (a.data[i] != b.data[i]) return false;
			return true;
		}
	return false;
}

bool operator!=(big_integer const &a, big_integer const &b) { return !(a == b); }
bool operator>(big_integer const &a, big_integer const &b) {
	if (a.sign == b.sign) {
		if (!a.sign) {
			big_integer pb = b;
			pb.sign = true;
			big_integer pa = a;
			pa.sign = true;
			if (pb.compare_abs(pb, pa) > 0)
				return true;
			return false;
		}
		else {
			big_integer pb = b;
			if (pb.compare_abs(a, b) > 0)
				return true;
			return false;
		}
	}
	else
		return a.sign;
}
bool operator<(big_integer const &a, big_integer const &b) { return !(a >= b); }
bool operator<=(big_integer const &a, big_integer const &b) { return !(a > b); }
bool operator>=(big_integer const &a, big_integer const &b) { return (a > b) || (a == b); }

std::string to_string(big_integer const &a) {
	std::string s;
	if (!a.sign) 
		s += '-';
	big_integer b = a;
	b.sign = true;
	big_integer cur = 0;
	std::vector<uint32_t> ans;
	if (b == 0) ans.push_back(0);
	while (b != 0) {
		cur = b % 10;
		ans.push_back(cur.data[0]);
		b /= 10;
	}
	std::reverse(ans.begin(), ans.end());
	for (size_t i = 0; i < ans.size(); i++) 
		s += std::to_string(ans[i]);
	return s;
}

std::ostream &operator<<(std::ostream &s, big_integer const &a) { return s << to_string(a); }

inline void big_integer::remove_leading_zeros() {
	while (this->data.size() != 1 && !this->data.back()) 
		this->data.pop_back();
}

inline void  big_integer::flip_bytes(big_integer &a) {
	for (size_t i = 0; i < a.data.size(); i++)
		a.data[i] = (~(a.data[i]) & static_cast<uint32_t>(BASE - 1));
}

big_integer  &big_integer::abstractLogicOperation(big_integer &a, big_integer b, uint32_t(*logicFunc)(uint32_t, uint32_t), bool(*check)(bool, bool)) {
	bool a_sign = a.sign;
	bool b_sign = b.sign;
	if (!a.sign) {
		flip_bytes(a);
		a.sign = true;
		a += 1;
	}
	if (!b.sign) {
		flip_bytes(b);
		b.sign = true;
		b += 1;
	}
	for (size_t i = 0; i < a.data.size(); i++)
		a.data[i] = logicFunc(a.data[i], (i < b.data.size() ? b.data[i] : 0));
	if (check(!a_sign, !b_sign)) {
		a -= 1;
		a.sign = false;
		flip_bytes(a);
	}
	return a;
}

int big_integer::compare_abs(big_integer const& a, big_integer const& b) {
	if (a == b)
		return 0;
	size_t max_size = std::max(a.data.size(), b.data.size());
	big_integer c;
	c.data.resize(max_size);
	int64_t r = 0;
	for (size_t i = 0; i < max_size; i++) {
		r = (i < a.data.size() ? static_cast<uint64_t>(a.data[i]) : 0) - (i < b.data.size() ? static_cast<uint64_t>(b.data[i]) : 0) + r;
		if (r < 0) {
			c.data[i] = static_cast<uint32_t>(r + BASE);
			r = -1;
		}
		else {
			c.data[i] = static_cast<uint32_t>(r);
			r = 0;
		}
	}
	if (r)
		return -1;
	else
		return 1;
}

big_integer big_integer::mul(big_integer const &b, uint32_t x) {
	int64_t carry = 0;
	big_integer a = b;
	a.data.push_back(0);
	for (size_t i = 0; i < a.data.size() || carry; i++) {
		int64_t cur = carry + static_cast<int64_t>(a.data[i]) * x;
		a.data[i] = static_cast<uint32_t>(cur % BASE);
		carry = static_cast<uint32_t>(cur / BASE);
	}
	while (a.data.size() > 1 && a.data.back() == 0)
		a.data.pop_back();
	return a;
}

inline void big_integer::make_zero() {
	if (data.size() == 0) data.push_back(0);
	if (data.size() == 1 && data[0] == 0)
		sign = true;
}


