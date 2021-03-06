#ifndef LRUCACHE_H
#define LRUCACHE_H

#include "cache.h"

template <class K, class V>
class LRUcache : public Cache<K, V> {
public:

	explicit LRUcache(unsigned __int64 size) : Cache(size), time(0) {}

private:

	unsigned __int64 time;
	std::map<_int64, K> pos;

	virtual void use_item(const K& key) {
		std::lock_guard<std::recursive_mutex> locker(_lock);
		auto temp = data.find(key);
		unsigned __int64 cnt = temp->second.second;
		auto value = temp->second.first;
		pos.erase(cnt);
		data.erase(key);
		insert_to(key, value);
	}

	virtual void insert_to_full(const K& key, const V& value) {
		std::lock_guard<std::recursive_mutex> locker(_lock);
		data.erase(pos.begin()->second);
		pos.erase(pos.begin());
		insert_to(key, value);
	}

	virtual void insert_to_not_full(const K& key, const V& value) {
		std::lock_guard<std::recursive_mutex> locker(_lock);
		insert_to(key, value);
	}

	virtual void replace_key(const K& key, const V& value) {
		std::lock_guard<std::recursive_mutex> locker(_lock);
		pos.erase(data.find(key)->second.second);
		data.erase(key);
		insert_to(key, value);
	}

	void insert_to(const K& key, const V& value) {
		pos.insert(std::make_pair(time, key));
		data.insert(std::make_pair(key, std::make_pair(value, time++)));
	}
};

#endif