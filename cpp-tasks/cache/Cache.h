#ifndef CACHE_H
#define CACHE_H

#include <set>
#include <map>
#include <mutex>

class CacheNotFoundException : std::exception {
public: CacheNotFoundException(char const* const msg) : exception(msg) {}
};

template <class K, class V>
class Cache {
protected:

	std::map<K, std::pair<V, unsigned __int64>> data;

	std::recursive_mutex _lock;

	unsigned __int64 max_size;
	unsigned __int64 cur_size;

	virtual void use_item(const K& key) = 0;
	virtual void insert_to_full(const K& key, const V& value) = 0;
	virtual void insert_to_not_full(const K& key, const V& value) = 0;
	virtual void replace_key(const K& key, const V& value) = 0; 

public:

	Cache(unsigned __int64 size) : max_size(size), cur_size(0) {}

	const V Get(const K& key) {
		auto it = data.find(key);
		if (it == data.end())
			throw CacheNotFoundException("Wrong key");
		auto ans = it->second.first;
		use_item(key);
		return ans;
	}

	void Put(const K& key, const V& value) {
		if (data.find(key) != data.end()) 
			replace_key(key, value);

		if (cur_size == max_size) 
			insert_to_full(key, value);
		else {
			insert_to_not_full(key, value);
			++cur_size;
		}
	}
};

#endif