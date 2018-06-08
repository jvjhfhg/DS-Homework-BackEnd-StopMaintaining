#pragma once

#include <fstream>
#include <map>
#include <cstdio>
using namespace std;

namespace sjtu {
    template <class Key, class T>
    class BPTree {
        const char *filename;
        std::map<Key, T> Tree;
        std::fstream io;

    public:
        BPTree(const char *f): filename(f) {
            io.open(filename, std::fstream::in);
            if (!io) {
                io.open(filename, std::fstream::out | std::fstream::binary);
                int cnt = 0;
                io.write(reinterpret_cast<const char *>(&cnt), sizeof cnt);
            }
            io.close();
            io.open(filename, std::fstream::in | std::fstream::out | std::fstream::binary);
            int cnt = 0;
            io.read(reinterpret_cast<char *>(&cnt), sizeof cnt);
            Key key;
            T val;
            for (int i = 0; i < cnt; ++i) {
                io.read(reinterpret_cast<char *>(&key), sizeof key);
                io.read(reinterpret_cast<char *>(&val), sizeof val);
                Tree.insert(std::make_pair(key, val));
            }
            io.close();
        }

        ~BPTree() {
            io.open(filename, std::fstream::out | std::fstream::binary);
            int cnt = Tree.size();
            io.write(reinterpret_cast<const char *>(&cnt), sizeof cnt);
            for (auto i : Tree) {
                io.write(reinterpret_cast<const char *>(&i.first), sizeof i.first);
                io.write(reinterpret_cast<const char *>(&i.second), sizeof i.second);
            }
            io.close();
        }

        void insert(const Key &key, const T &val) {
            Tree.insert(std::make_pair(key, val));
        }

        pair<T, bool> query(const Key &key) const {
            if (!Tree.count(key))
                return pair<T, bool>(T(), false);
            return pair<T, bool>(Tree.find(key)->second, true);
        }

        void modify(const Key &key, const T &val) {
            Tree[key] = val;
        }

        void erase(const Key &key) {
            Tree.erase(Tree.find(key));
        }

        vector<pair<Key, T>> traverse() const {
            vector<pair<Key, T>> res;
            for (auto i : Tree)
                res.push_back(i);
            return res;
        }

        void clear() {
            Tree.clear();
        }
    };
}
