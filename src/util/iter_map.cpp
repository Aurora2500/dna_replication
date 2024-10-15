#include "iter_map.hpp"

template<typename T>
std::size_t IteratorHash<T>::operator()(const typename std::list<T>::iterator& it) const noexcept {
		std::hash<T*> h;
		return h(&(*it));
}
