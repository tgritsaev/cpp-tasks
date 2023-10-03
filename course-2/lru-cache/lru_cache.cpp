#include "lru_cache.h"

LruCache::LruCache(size_t max_size) : max_size_(max_size) {
}

void LruCache::Set(const std::string& key, const std::string& value) {
    if (map_.find(key) != map_.end()) {
        list_.erase(map_[key]);
    }
    ListElement a(key, value);
    list_.push_back(a);
    map_[key] = (--list_.end());
    if (list_.size() > max_size_) {
        std::string& str = list_.begin()->key;
        std::cout << "erase " << str << '\n';
        map_.erase(str);
        list_.pop_front();
    }
    std::cout << list_.size() << '\n';
}

bool LruCache::Get(const std::string& key, std::string* value) {
    std::cout << (map_.find(key) == map_.end()) << '\n';
    if (map_.find(key) == map_.end()) {
        return false;
    }
    *value = map_[key]->value;
    list_.erase(map_[key]);
    list_.push_back(ListElement{key, *value});
    map_[key] = (--list_.end());
    return true;
}
