#pragma once

#include <fstream>
#include "map.hpp"
#include "utility.hpp"
#include "vector.hpp"

namespace sjtu {
    template <class Key, class T, class Compare = std::less<Key>>
    class FileMap {
        const char *filename;
        // std::map<Key, T, Compare> Tree;
        map<Key, T, Compare> Tree;
        std::fstream io;

    public:
        FileMap(const char *f): filename(f) {
            io.open(filename, std::fstream::in);
            if (!io) {
                io.open(filename, std::fstream::out | std::fstream::binary);
                int cnt = 0;
                io.write((const char *)(&cnt), sizeof cnt);
            }
            io.close();
            io.open(filename, std::fstream::in | std::fstream::out | std::fstream::binary);
            int cnt = 0;
            io.read((char *)(&cnt), sizeof cnt);
            Key key;
            T val;
            for (int i = 0; i < cnt; ++i) {
                io.read((char *)(&key), sizeof key);
                io.read((char *)(&val), sizeof val);
                Tree.insert(make_pair(key, val));
            }
            io.close();
        }

        ~FileMap() {
            io.open(filename, std::fstream::out | std::fstream::binary);
            int cnt = Tree.size();
            io.write((const char *)(&cnt), sizeof cnt);
            for (auto i = Tree.begin(); i != Tree.end(); ++i) {
                io.write((const char *)(&i->first), sizeof i->first);
                io.write((const char *)(&i->second), sizeof i->second);
            }
            io.close();
        }

        void insert(const Key &key, const T &val) {
            Tree.insert(make_pair(key, val));
        }

        pair<T, bool> query(const Key &key) {
            if (!Tree.count(key))
                return make_pair(T(), false);
            return make_pair(Tree.find(key)->second, true);
        }

        void modify(const Key &key, const T &val) {
            Tree[key] = val;
        }

        void erase(const Key &key) {
            Tree.erase(Tree.find(key));
        }

        vector<pair<Key, T>> traverse() {
            vector<pair<Key, T>> res;
            for (auto i = Tree.begin(); i != Tree.end(); ++i)
                res.push_back(*i);
            return res;
        }
        
        int count(const Key &key) {
            return Tree.count(key);
        }
        
        int Size() const {
            return Tree.size();
        }

        void clear() {
            Tree.clear();
        }
    };
}
