#ifndef LFUCACHE_H
#define LFUCACHE_H

#include "cache.h"

template <class K, class V>
class LFUcache : public Cache<K, V> {
public:

	explicit LFUcache(unsigned __int64 size) : Cache(size) {}

private:

	std::multimap<unsigned __int64, K> pos;

	virtual void use_item(const K& key) {
		std::lock_guard<std::recursive_mutex> locker(_lock);
		auto temp = data.find(key);
		unsigned __int64 cnt = temp->second.second;
		pos.insert(std::make_pair(cnt + 1, key));
		auto value = temp->second.first;
		data.erase(key);
		data.insert(std::make_pair(key, std::make_pair(value, cnt + 1)));
	}

	virtual void insert_to_full(const K& key, const V& value) {
		std::lock_guard<std::recursive_mutex> locker(_lock);
		while (!data.count(pos.begin()->second) || data.find(pos.begin()->second)->second.second != pos.begin()->first)
			pos.erase(pos.begin());
		data.erase(pos.begin()->second);
		pos.erase(pos.begin()->second);
		insert_to(key, value);
	}

	virtual void insert_to_not_full(const K& key, const V& value) {
		std::lock_guard<std::recursive_mutex> locker(_lock);
		insert_to(key, value);
	}

	virtual void replace_key(const K& key, const V& value) {
		std::lock_guard<std::recursive_mutex> locker(_lock);
		data.erase(key);
		insert_to(key, value);
	}

	void insert_to(const K& key, const V& value) {
		pos.insert(std::make_pair(0, key));
		data.insert(std::make_pair(key, std::make_pair(value, 0)));
	}
};

#endif