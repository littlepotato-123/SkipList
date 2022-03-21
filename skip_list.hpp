#ifndef SKIPLIST_H
#define SKIPLIST_H

#include <iostream>
#include <cstdlib>
#include <cmath>
#include <memory>
#include <string>
#include <mutex>
#include <fstream>
#include <vector>
#include <sstream>
#define STORE_FILE "./dumpFile.txt"
#define DELIMITER ":"
//std::mutex mtx;
//std::string DELIMITER = ":";

template<class K, class V>
class Node {

public:

    Node() {}
    
    Node(K k, V v, int);

    ~Node();

    K get_key() const;

    V get_value() const;

    void set_value(V);

    std::vector<std::shared_ptr<Node<K, V>> > forward;

    int node_level;

private:
    K key;
    V value;
};

template<class K, class V> 
Node<K, V>::Node(const K k, const V v, int level) {
    this->key = k;
    this->value = v;
    this->node_level = level;

    this->forward.resize(level + 1);
}

template<class K, class V> 
Node<K, V>::~Node() {}

template<class K, class V> 
K Node<K, V>::get_key() const {
    return key;
};

template<class K, class V> 
V Node<K, V>::get_value() const {
    return value;
};

template<class K, class V> 
void Node<K, V>::set_value(V value) {
    this->value=value;
};

template <class K, class V> 
class SkipList {

public:
    SkipList(int);
    ~SkipList();
    int get_random_level();
    std::shared_ptr<Node<K, V>> create_node(K, V, int);
    int insert_element(K, V);
    void display_list();
    bool search_element(K);
    void delete_element(K);
    void dump_file();
    void load_file();
    int size();

private:
    bool is_valid_string(const std::string& str);
    void get_key_value_from_string(const std::string& str, std::string& key, std::string& value);

private:
    int _max_level;
    int _skip_list_level;
    std::shared_ptr<Node<K, V>> _header;

    std::ofstream ofs;
    std::ifstream ifs;
    std::mutex mtx_;
    int _element_count;
};

template<typename K, typename V>
std::shared_ptr<Node<K, V>> SkipList<K, V>::create_node(const K k, const V v, int level) {
    return std::make_shared<Node<K, V>>(k, v, level);
}

template<typename K, typename V>
int SkipList<K, V>::insert_element(const K key, const V value) {
    //mtx.lock();
    std::lock_guard<std::mutex> locker(mtx_);
    auto current = this->_header;
    std::vector<std::shared_ptr<Node<K, V>>> update(_max_level+1);
    //Node<K, V>* update[_max_level+1];
    //memset(update, 0, sizeof(Node<K, V>*)*(_max_level+1)); 

    for(int i = _skip_list_level; i >= 0; --i) {
        while(current->forward[i] && current->forward[i]->get_key() < key) {
            current = current->forward[i];
        }
        update[i] = current;
    } 
    current = current->forward[0];

    if(current && current->get_key() == key) {
        //std::cout << "key: " << key << ", exists" << std::endl;
        //mtx.unlock();
        return 1;
    }

    if (!current || current->get_key() != key ) {
        int random_level = get_random_level();

        if (random_level > _skip_list_level) {
            for(int i = _skip_list_level + 1; i < random_level + 1; ++i) {
                update[i] = _header;
            }
            _skip_list_level = random_level;
        }   

        auto inserted_node = create_node(key, value, random_level);

        for (int i = 0; i <= random_level; i++) {
            inserted_node->forward[i] = update[i]->forward[i];
            update[i]->forward[i] = inserted_node;
        }
        //std::cout << "Successfully inserted key:" << key << ", value:" << value << std::endl;
        ++_element_count;
    }
    //mtx.unlock();
    return 0;
}

template<typename K, typename V> 
void SkipList<K, V>::display_list() {
    std::cout << "\n*****Skip List*****"<<"\n"; 
    for (int i = 0; i <= _skip_list_level; i++) {
        auto node = this->_header->forward[i];
        std::cout << "Level " << i << ": ";
        while(node) {
            std::cout << node->get_key() << ":" << node->get_value() << ";";
            node = node->forward[i];
        }
        std::cout << std::endl;
    }
}

template<typename K, typename V> 
void SkipList<K, V>::dump_file() { 
    std::cout << "dump_file-----------------" << std::endl;
    ofs.open(STORE_FILE);
    auto node = this->_header->forward[0]; 

    while(node) {
        ofs << node->get_key() << ":" << node->get_value() << "\n";
        std::cout << node->get_key() << ":" << node->get_value() << ";\n";
        node = node->forward[0];
    }

    ofs.flush();
    ofs.close();
    return;
}

template<typename K, typename V> 
void SkipList<K, V>::load_file() {
    ifs.open(STORE_FILE);
    std::cout << "load_file-----------------" << std::endl;
    std::string line;
    std::string key;
    std::string value;
    
    while (getline(ifs, line)) {
        get_key_value_from_string(line, key, value);
        if (key.empty() || value.empty()) {
            continue;
        }
        K real_key;
        V real_value;
        {
            std::stringstream ss;
            ss << key;
            ss >> real_key;
            ss.clear();
            ss << value;
            ss >> real_value;
        }
        insert_element(real_key, real_value);
        std::cout << "key:" << key << "value:" << value << std::endl;
    }
    ifs.close();
}

template<typename K, typename V> 
int SkipList<K, V>::size() { 
    return _element_count;
}

template<typename K, typename V>
void SkipList<K, V>::get_key_value_from_string(const std::string& str, std::string& key, std::string& value) {

    if(!is_valid_string(str)) {
        return;
    }
    key = str.substr(0, str.find(DELIMITER));
    value = str.substr(str.find(DELIMITER)+1, str.length());
}

template<typename K, typename V>
bool SkipList<K, V>::is_valid_string(const std::string& str) {
   
    if (str.empty()) {
        return false;
    }
    if (str.find(DELIMITER) == std::string::npos) {
        return false;
    }
    return true;
}

template<typename K, typename V> 
void SkipList<K, V>::delete_element(K key) {
    
    //mtx.lock();
    std::lock_guard<std::mutex> locker(mtx_);
    auto current = this->_header; 
    std::vector<std::shared_ptr<Node<K, V>>> update(_max_level + 1);
    // Node<K, V>* update[_max_level + 1];
    // memset(update, 0, sizeof(Node<K, V>*)*(_max_level+1));

    for (int i = _skip_list_level; i >= 0; i--) {
        while (current->forward[i] && current->forward[i]->get_key() < key) {
            current = current->forward[i];
        }
        update[i] = current;
    }
    current = current->forward[0];

    if (current && current->get_key() == key) {
        
        //从底部往上找
        for (int i = 0; i <= _skip_list_level; i++) {
            if (update[i]->forward[i] != current) break;
            update[i]->forward[i] = current->forward[i];
        }
        while(_skip_list_level > 0 && !_header->forward[_skip_list_level]) {
            --_skip_list_level;
        }

        std::cout << "Successfully deleted key "<< key << std::endl;
        --_element_count;
    }
    //mtx.unlock();
    return;
}

template<typename K, typename V> 
bool SkipList<K, V>::search_element(K key) {
    //std::cout << "search_element-----------------" << std::endl;
    auto current = _header;

    for(int i = _skip_list_level; i >= 0; --i) {
        while(current->forward[i] && current->forward[i]->get_key() < key) {
            current = current->forward[i];
        }
    }
    current = current->forward[0];
    if (current && current->get_key() == key) {
        //std::cout << "Found key: " << key << ", value: " << current->get_value() << std::endl;
        return true;
    }
    //std::cout << "Not Found Key:" << key << std::endl;
    return false;
}

template<typename K, typename V> 
SkipList<K, V>::SkipList(int max_level) {
    this->_max_level = max_level;
    this->_skip_list_level = 0;
    this->_element_count = 0;
    K k;
    V v;
    this->_header = std::make_shared<Node<K, V>>(k, v, _max_level);
}

template<typename K, typename V> 
SkipList<K, V>::~SkipList() {
    if(ofs.is_open()) ofs.close();
    if(ifs.is_open()) ifs.close();
}

template<typename K, typename V>
int SkipList<K, V>::get_random_level(){
    
    int k = 1;
    while (rand() % 2) {
        ++k;
    }
    k = (k < _max_level) ? k : _max_level;
    return k;
};
#endif