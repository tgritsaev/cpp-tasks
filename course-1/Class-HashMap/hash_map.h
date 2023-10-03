// #pragma once
#include <list>
#include <vector>
#include <stdexcept>

// hope for the best

template<class KeyType, class ValueType, class Hash = std::hash<KeyType> >
class HashMap {
public:
    using iterator = typename std::list<std::pair<const KeyType, ValueType>>::iterator;
    using const_iterator = typename std::list<std::pair<const KeyType, ValueType>>::const_iterator;

private:
    size_t Size = 0, Capacity = 0;

    std::list<std::pair<const KeyType, ValueType>> values;
    std::vector<std::list<iterator>> table;

    Hash hasher;

    void expand() {
        table.clear();
        Capacity *= 2;
        table.resize(Capacity);
        for (iterator it = values.begin(); it != values.end(); ++it) {
            int pos = hasher(it->first) % Capacity;
            table[pos].push_back(it);
        }
    }

public:

    HashMap(const Hash& hasher_ = Hash()): hasher(hasher_) {
        // hasher = hasher_;
        Capacity = 1;
        table.resize(1);
        Size = 0;
    }

    template<class Iterator>
    HashMap(const Iterator start, const Iterator end, const Hash& hasher_ = Hash()): hasher(hasher_) {
        Capacity = std::distance(start, end);
        table.resize(Capacity);
        for (Iterator it = start; it != end; ++it) {
            insert(*it);
        }
    }

    HashMap(const std::initializer_list<std::pair<const KeyType, ValueType>>& elements,
            const Hash& hasher_ = Hash()) : HashMap(elements.begin(), elements.end(), hasher_) {}

    HashMap<KeyType, ValueType, Hash>& operator=(const HashMap<KeyType, ValueType, Hash>& other) {
        if (begin() != other.begin()) {
            clear();
            new (this) HashMap(other.values.begin(), other.values.end(), other.hasher);
        }
        return *this;
    }

    size_t size() const {
        return Size;
    }

    bool empty() const {
        return (Size == 0);
    }

    Hash hash_function() const {
        return hasher;
    }

    void insert(const std::pair<const KeyType, ValueType>& element) {
        if (find(element.first) == end()) {
            if (++Size == Capacity) {
                expand();
            }
            int pos = hasher(element.first) % Capacity;
            values.push_front(element);
            table[pos].push_back(values.begin());
        }
    }

    void erase(const KeyType& key) {
        int pos = hasher(key) % Capacity;

        for (auto it = table[pos].begin(); it != table[pos].end(); ++it) {
            if ((*it)->first == key) {
                values.erase(*it);
                table[pos].erase(it);
                --Size;
                return;
            }
        }
    }

    // template doesn't help here

    const_iterator find(const KeyType& key) const  {
        int pos = hasher(key) % Capacity;
        for (const const_iterator& it : table[pos]) {
            if (it->first == key) {
                return it;
            }
        }
        return values.end();
    }

    iterator find(const KeyType& key) {
        int pos = hasher(key) % Capacity;
        for (iterator& it : table[pos]) {
            if (it->first == key) {
                return it;
            }
        }
        return values.end();
    }

    ValueType& operator[](const KeyType& key) {
        if (find(key) == end()) {
            insert({key, ValueType()});
        }
        return find(key)->second;
    }

    const ValueType& at(const KeyType& key) const {
        if (find(key) == end()) {
            throw std::out_of_range("There is no such key whereas operator 'at' require.\n");
        }
        return find(key)->second;
    }

    void clear() {
        Size = 0;
        Capacity = 1;
        values.clear();
        table.clear();
        table.resize(1);
    }

    iterator begin() {
        return values.begin();
    }

    iterator end() {
        return values.end();
    }

    const_iterator begin() const {
        return values.cbegin();
    }

    const_iterator end() const {
        return values.cend();
    }
};
