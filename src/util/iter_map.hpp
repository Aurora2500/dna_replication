#pragma once

#include <unordered_map>
#include <list>

template <typename T>
using node = std::list<T>::iterator;

template<typename T>
struct IteratorHash {
	std::size_t operator()(const typename std::list<T>::iterator& it) const noexcept;
};

template<typename K, typename V>
using iter_map = std::unordered_map<std::list::<K>::iterator, V, IteratorHash<K>>;