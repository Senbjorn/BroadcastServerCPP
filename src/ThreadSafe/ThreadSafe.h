#ifndef BROADCASTSERVER_THREADSAFE_H
#define BROADCASTSERVER_THREADSAFE_H

#include <unordered_map>
#include <mutex>
#include <condition_variable>
#include <utility>

/*
*
* Value, thread safe type
* Concurrent iterators
*/
template<typename Key, typename Value>
class threadsafe_map {
private:
	std::unordered_map<Key, Value> map;
	std::mutex mutex;
	std::condition_variable cond;
public:
	threadsafe_list() = default;
	threadsafe_list(const threadsafe_list& other) = delete;
	threadsafe_list & operator=(const threadsafe_list & other) = delete;
	threadsafe_list(std::list<T> other_list): list(other_list) {}
	
	bool empty() const;
	size_t size() const;
	size_t count(const Key& k) const;
	void insert(const std::pair<const Key, Value>& v);
	void insert(std::pair<const Key, Value>&& v);
	void erase(const Key& k);
	void erase(Key&& k);
	Value& find(const Key& k);
	Value& find(Key&& k);
	Value& operator[](const Key& k);
	Value& operator[](Key&& k);
	
	bool empty() const {
		std::lock_guard<std::mutex> lock(mutex);
		return map.empty();
	}

	size_t size() const {
		std::lock_guard<std::mutex> lock(mutex);
		return map.size();
	}

	size_t count(const Key& k) const {
		std::lock_guard<std::mutex> lock(mutex);
		return map.count(k);
	}

	void insert(const std::pair<const Key, Value>& v) {
		std::lock_guard<std::mutex> lock(mutex);
		map.insert(v);
	}

	void insert(std::pair<const Key, Value>&& v) {
		std::lock_guard<std::mutex> lock(mutex);
		map.insert(std::move(v));
	}

	Value& find(const Key& k) {
		std::lock_guard<std::mutex> lock(mutex);
		return *map.find(k);
	}

	Value& find(Key&& k) {
		std::lock_guard<std::mutex> lock(mutex);
		return *map.find(std::move(k));
	}

	Value& operator[](const Key& k) {
		std::lock_guard<std::mutex> lock(mutex);
		return map[k];
	}

	Value& operator[](Key&& k) {
		std::lock_guard<std::mutex> lock(mutex);
		return map[std::move(k)];
	}

	
};

#endif
