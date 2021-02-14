#ifndef HASHMAP_HPP
#define HASHMAP_HPP
#include <cstdint>
#include <cstddef>
#include <cstdio>
#include <functional>
#include <algorithm>
#include <cstring>
#define LOAD_FACTOR(x) (((x) * 775) >> 10)
template <typename K, typename V>
class HashNode {
public:
    K key;
    V val;
    uint64_t distance;
    bool used;
};
template <typename K >
class HashFn {
public:
    size_t operator()(K key) {
        return std::hash<K>{}(key);
    }
};

template <typename K, typename V, typename F = HashFn<K> >
class HashMap {
private:
        size_t capacity = 8;
        HashNode<K, V> *init_table;
        F hash_func;
        uint64_t max_distance = 0;
        size_t size = 0;
public:
    HashMap() {
        init_table = new HashNode<K, V>[capacity + 1]();
        memset(init_table, 0x0, (sizeof(HashNode<K, V>) * capacity));
    }
    class Iterator;
    Iterator begin() {
        return Iterator(this->init_table, capacity);
    }
    Iterator end() {
        return Iterator(nullptr);
    }
    Iterator find(K key) {
        return Iterator(this->init_table, key, capacity);
    }
    class Iterator {
    public:
    using pointer = HashNode<K, V> *;
    Iterator (const pointer node, size_t table_capacity = 0) 
        noexcept : hashnode(node), capacity(table_capacity) {}
    
    Iterator& operator=(pointer hashNode) {
		return *this;
	}
    Iterator& operator++() {
        if(capacity <= index) {
            hashnode = nullptr;
            *this = nullptr;
        }
        if(hashnode) index++;
        return *this;
    }
    Iterator operator++(int) {
        Iterator iterator = *this;
        ++*this;
        return iterator;
    }
    Iterator (const pointer node, K key, size_t table_capacity = 0) 
        noexcept : hashnode(node), capacity(table_capacity) {
        if(node){
            for(int ii = 0; ii < capacity; ii ++) {
                pointer hash_node = &hashnode[ii];
                if(hash_node->key == key) {
                    index = ii;
                    return;
                }
            }
        }
        *this = nullptr;
        hashnode = nullptr;
    }

    bool operator!=(const Iterator& iterator) {
        return iterator.hashnode != hashnode;
	}
    const pointer operator->() {
        return &hashnode[index];
    }
    private:
        pointer hashnode;
        size_t index = 0;
        size_t capacity = 0;
    };
    void resize_capacity() {
        if(size >= LOAD_FACTOR(capacity)) {
            size = 0;
            size_t oldcapacity = capacity;
            capacity = capacity * 2;
            HashNode<K, V>* oldentries = init_table;
            init_table = new HashNode<K, V>[capacity + 1];
            memset(init_table, 0x0, (sizeof(HashNode<K, V>) * capacity));
            for(int ii = 0; ii < oldcapacity; ii++) {
                HashNode<K, V>* tbl = &oldentries[ii];
                if(tbl->used)
                    insert(tbl->key, tbl->val);
            }
            delete []oldentries;
        }
        return;
    }
    ~HashMap() {
        delete []init_table;
    }
    /// Doing optimization for TCO
    #pragma GCC push_options
    #pragma GCC optimize ("O0")
    void insert (K key, V val, uint64_t inserting_distance = 0) {
        /// checking for capacity percentage
        resize_capacity();
        size_t hash_index = hash_func(key) % capacity;
        HashNode<K, V>* node = &init_table[hash_index];
        size+=1;
        while(node->used && node->key != key) {
            uint64_t probing_distance = node->distance;
            max_distance = std::max(max_distance, inserting_distance);
            if(probing_distance < inserting_distance) {
                HashNode<K, V> swapped_node = *node;
                node->key = key;
                node->val = val;
                node->distance = inserting_distance;
                insert(swapped_node.key, swapped_node.val, swapped_node.distance);
            } else {
                hash_index += (hash_index == capacity) ? hash_index % capacity : 1;
                node = &init_table[hash_index];
                inserting_distance+=1;
                continue;
            }
        }
        max_distance = std::max(max_distance, inserting_distance);
        node->key = key;
        node->val = val;
        node->used = true;
        node->distance = inserting_distance;
    }
    #pragma GCC pop_options
    HashNode<K, V>* get(K key) {
        size_t hash_index = hash_func(key) % capacity;
        for(int ii = 0; ii <= max_distance; ii++) {
            HashNode<K, V>* node = &init_table[(hash_index + ii) % capacity];
            if(!node->used) return nullptr;
            if(node->key == key) {
                return node;
            }
        }
        return nullptr;
    }
};
#endif
