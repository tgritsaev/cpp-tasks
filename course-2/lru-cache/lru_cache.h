#pragma once

#include <string>
#include <unordered_map>
#include <list>
#include <iterator>
#include <iostream>

class LruCache {
public:
    LruCache(size_t max_size);

    void Set(const std::string& key, const std::string& value);

    bool Get(const std::string& key, std::string* value);

private:
    struct ListElement {
        ListElement(const std::string& k, const std::string& v) : key(k), value(v) {
        }
        std::string key;
        std::string value;
    };

    size_t max_size_;
    std::unordered_map<std::string, std::list<ListElement>::iterator> map_;
    std::list<ListElement> list_;
};
