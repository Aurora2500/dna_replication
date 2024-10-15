#pragma once

#include <unordered_map>
#include <list>

template <typename T>
using node = typename std::list<T>::iterator;

template<typename T>
struct IteratorHash {
	inline std::size_t operator()(const typename std::list<T>::iterator& it) const noexcept {
		return std::hash<T*>()(&(*it));
	}
};

template<typename K, typename V>
using iter_map = std::unordered_map<typename std::list<K>::iterator, V, IteratorHash<K>>;