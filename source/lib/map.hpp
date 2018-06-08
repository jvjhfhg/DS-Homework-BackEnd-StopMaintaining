#pragma once

#include <functional>
#include <cstddef>
#include "utility.hpp"
#include "exceptions.hpp"

namespace sjtu {
    template <
        class KeyType, 
        class Type, 
        class Compare = std::less<KeyType>
    >
    class map {
      public:
        typedef pair<const KeyType, Type> value_type;
        
      private:
        int _cmp(const KeyType &a, const KeyType &b) const {
            static Compare cmp;
            return (int)cmp(b, a) - cmp(a, b);
        }
        
      private:
        //struct LinkNode;
        struct SplayNode;
        
        //typedef LinkNode LNode;
        typedef SplayNode SNode;
        
        /*struct LinkNode {
            LNode *_pre, *_nxt;
            SNode *_tp;
            
            LinkNode(SNode *tp): _tp(tp) {
                _pre = _nxt = nullptr;
            }
            
            void _insert(LNode *p, size_t ch) {
                if (p == nullptr) return;
                if (ch == 0) {
                    if (_pre) _pre->_nxt = p;
                    p->_pre = _pre;
                    _pre = p;
                    p->_nxt = this;
                } else {
                    if (_nxt) _nxt->_pre = p;
                    p->_nxt = _nxt;
                    _nxt = p;
                    p->_pre = this;
                }
            }
            
            void _del() {
                if (_pre) _pre->_nxt = _nxt;
                if (_nxt) _nxt->_pre = _pre;
                delete this;
            }
        } *_head, *_tail;*/
        
        struct SplayNode {
            SNode *_ch[2], *_fa;
            value_type _val;
            
            //LNode *_lp;
            SNode *_pre, *_nxt;
            
            SplayNode(const value_type &val): _val(val) {
                _ch[0] = _ch[1] = _fa = nullptr;
                //_lp = new LNode(this);
                _pre = _nxt = nullptr;
            }
            
            inline int _which() {
                return this == _fa->_ch[1];
            }
            
            void _insert(SNode *p, int ch) {
                if (p == nullptr) return;
                if (ch == 0) {
                    if (_pre) _pre->_nxt = p;
                    p->_pre = _pre;
                    _pre = p;
                    p->_nxt = this;
                } else {
                    if (_nxt) _nxt->_pre = p;
                    p->_nxt = _nxt;
                    _nxt = p;
                    p->_pre = this;
                }
            }
            
            void _del() {
                if (_pre) _pre->_nxt = _nxt;
                if (_nxt) _nxt->_pre = _pre;
            }
        }; mutable SNode *_root;
        SNode *_head, *_tail;
        
        size_t _size;
        
        void _rotate(SNode *k) const {
            SNode *p = k->_fa;
            size_t l = k->_which(), r = l ^ 1;
            
            k->_fa = p->_fa;
            if (p->_fa) p->_fa->_ch[p->_which()] = k;
            
            p->_ch[l] = k->_ch[r];
            if (k->_ch[r]) k->_ch[r]->_fa = p;
            
            k->_ch[r] = p;
            p->_fa = k;
        }
        
        void _splay(SNode *k, const SNode *aim_fa = nullptr) const {
            while (k->_fa != aim_fa) {
                SNode *p = k->_fa;
                if (p->_fa != aim_fa) {
                    if (k->_which() ^ p->_which()) _rotate(k);
                    else _rotate(p);
                }
                _rotate(k);
            }
            
            if (aim_fa == nullptr) _root = k;
        }
        
        SNode *_find(const KeyType &key) const {
            SNode *k = _root;
            while (k) {
                if (!_cmp(key, k->_val.first)) {
                    _splay(k);
                    return k;
                }
                k = k->_ch[_cmp(key, k->_val.first) == 1];
            }
            return nullptr;
        }
        
        pair<SNode *, bool> _insert(const value_type &val) {
            SNode *k = _root, *p = nullptr;
            size_t drct;
            
            while (k) {
                p = k;
                if (!_cmp(val.first, k->_val.first)) return pair<SNode *, bool>(k, false);
                drct = (_cmp(val.first, k->_val.first) == 1);
                k = k->_ch[drct];
            }
            
            ++_size;
            
            k = new SNode(val);
            if (!p) {
                _root = k;
            } else {
                k->_fa = p;
                p->_ch[drct] = k;
                p->_insert(k, drct);
                _splay(k);
            }
            
            if (k->_pre == nullptr) _head = k;
            if (k->_nxt == nullptr) _tail = k;
            
            return pair<SNode *, bool>(k, true);
        }
        
        void _erase(SNode *k) {
            --_size;
            
            _splay(k);
            
            if (k->_ch[0] == nullptr) {
                _root = k->_ch[1];
                if (_root) _root->_fa = nullptr;
            } else if (k->_ch[1] == nullptr) {
                _root = k->_ch[0];
                if (_root) _root->_fa = nullptr;
            } else {
                _splay(k->_pre, k);
                
                _root = k->_ch[0];
                _root->_fa = nullptr;
                k->_ch[1]->_fa = _root;
                _root->_ch[1] = k->_ch[1];
            }
            
            if (_head == k) _head = k->_nxt;
            if (_tail == k) _tail = k->_pre;
            
            k->_del();
            
            delete k;
        }
        
        /*void _copy(SNode *&k, const SNode *p, SNode *fa = nullptr) {
            if (p == nullptr) return;
            
            k = new SNode(p->_val);
            k->_fa = fa;
            
            _copy(k->_ch[0], p->_ch[0], k);
            
            k->_insert(_tail, 0);
            if (_tail == nullptr) _head = k;
            
            _tail = k;
            
            _copy(k->_ch[1], p->_ch[1], k);
        }*/
        
        void _delete(SNode *k) {
            if (k == nullptr) return;
            for (size_t i = 0; i < 2; ++i)
                _delete(k->_ch[i]);
            //delete k->_lp;
            delete k;
        }
        
      public:
        class const_iterator;
        class iterator {
            friend map;
            
          private:
            SNode *_pos;
            map *_container;
            
          public:
            iterator(const map *container = nullptr): _container((map *)container) {
                _pos = nullptr;
            }
            
            iterator(const iterator &oth) {
                _container = oth._container;
                _pos = oth._pos;
            }
            
            iterator operator ++ (int) {
                iterator res = *this;
                ++*this;
                return res;
            }
            
            iterator &operator ++ () {
                if (_pos == nullptr)
                    throw invalid_iterator();
                _pos = _pos->_nxt;
                return *this;
            }
            
            iterator operator -- (int) {
                iterator res = *this;
                --*this;
                return res;
            }
            
            iterator &operator -- () {
                if ((_pos && _pos->_pre == nullptr) || _container->_tail == nullptr)
                    throw invalid_iterator();
                _pos = (_pos == nullptr ? _container->_tail : _pos->_pre);
                return *this;
            }
            
            value_type &operator * () {
                if (_pos == nullptr)
                    throw invalid_iterator();
                return _pos->_val;
            }
            
            bool operator == (const iterator &oth) const {
                return _container == oth._container && _pos == oth._pos;
            }
            
            bool operator == (const const_iterator &oth) const {
                return _container == oth._container && _pos == oth._pos;
            }
            
            bool operator != (const iterator &oth) const {
                return !(*this == oth);
            }
            
            bool operator != (const const_iterator &oth) const {
                return !(*this == oth);
            }
            
            value_type *operator -> () const noexcept {
                if (_pos == nullptr)
                    throw invalid_iterator();
                return &(_pos->_val);
            }
        };
        
        class const_iterator {
            friend map;
            
          private:
            SNode *_pos;
            map *_container;
            
          public:
            const_iterator(const map *container = nullptr): _container((map *)container) {
                _pos = nullptr;
            }
            
            const_iterator(const const_iterator &oth) {
                _container = oth._container;
                _pos = oth._pos;
            }
            
            const_iterator(const iterator &oth) {
                _container = oth._container;
                _pos = oth._pos;
            }
            
            const_iterator operator ++ (int) {
                const_iterator res = *this;
                ++*this;
                return res;
            }
            
            const_iterator &operator ++ () {
                if (_pos == nullptr)
                    throw invalid_iterator();
                _pos = _pos->_nxt;
                return *this;
            }
            
            const_iterator operator -- (int) {
                const_iterator res = *this;
                --*this;
                return res;
            }
            
            const_iterator &operator -- () {
                if ((_pos && _pos->_pre == nullptr) || _container->_tail == nullptr)
                    throw invalid_iterator();
                _pos = (_pos == nullptr ? _container->_tail : _pos->_pre);
                return *this;
            }
            
            const value_type &operator * () {
                if (_pos == nullptr)
                    throw invalid_iterator();
                return _pos->_val;
            }
            
            bool operator == (const iterator &oth) const {
                return _container == oth._container && _pos == oth._pos;
            }
            
            bool operator == (const const_iterator &oth) const {
                return _container == oth._container && _pos == oth._pos;
            }
            
            bool operator != (const iterator &oth) const {
                return !(*this == oth);
            }
            
            bool operator != (const const_iterator &oth) const {
                return !(*this == oth);
            }
            
            const value_type *operator -> () const noexcept {
                if (_pos == nullptr)
                    throw invalid_iterator();
                return &(_pos->_val);
            }
        };
        
        map() {
            _root = nullptr;
            _head = _tail = nullptr;
            _size = 0;
        }
        
        map(const map &oth) {
            _root = nullptr;
            _head = _tail = nullptr;
            //_copy(_root, oth._root);
            for (auto it = oth._head; it; it = it->_nxt)
                _insert(it->_val);
            _size = oth._size;
        }
        
        map &operator = (const map &oth) {
            if (this == &oth) return *this;
            _delete(_root);
            _root = nullptr;
            _head = _tail = nullptr;
            //_copy(_root, oth._root);
            for (auto it = oth._head; it; it = it->_nxt)
                _insert(it->_val);
            _size = oth._size;
            return *this;
        }
        
        ~map() {
            _delete(_root);
        }
        
        Type &at(const KeyType &key) {
            SNode *k = _find(key);
            if (!k) throw index_out_of_bound();
            return k->_val.second;
        }
        
        const Type &at(const KeyType &key) const {
            SNode *k = _find(key);
            if (!k) throw index_out_of_bound();
            return k->_val.second;
        }
        
        Type &operator [] (const KeyType &key) {
            SNode *k = _find(key);
            if (!k) k = _insert(value_type(key, Type())).first;
            return k->_val.second;
        }
        
        const Type &operator [] (const KeyType &key) const {
            SNode *k = _find(key);
            if (!k) throw index_out_of_bound();
            return k->_val.second;
        }
        
        iterator begin() {
            iterator res(this);
            res._pos = _head;
            return res;
        }
        
        const_iterator cbegin() const {
            const_iterator res(this);
            res._pos = _head;
            return res;
        }
        
        iterator end() {
            iterator res(this);
            res._pos = nullptr;
            return res;
        }
        
        const_iterator cend() const {
            const_iterator res(this);
            res._pos = nullptr;
            return res;
        }
        
        bool empty() const {
            return _size == 0;
        }
        
        size_t size() const {
            return _size;
        }
        
        void clear() {
            _delete(_root);
            _root = nullptr;
            _head = _tail = nullptr;
            _size = 0;
        }
        
        pair<iterator, bool> insert(const value_type &val) {
            pair<iterator, bool> res;
            res.first._container = (map *)this;
            
            auto k = _insert(val);
            res.first._pos = k.first;
            res.second = k.second;
            return res;
        }
        
        void erase(iterator itr) {
            if (itr._container != this || itr._pos == nullptr)
                throw invalid_iterator();
            _erase(itr._pos);
        }
        
        size_t count(const KeyType &key) const {
            return _find(key) != nullptr;
        }
        
        iterator find(const KeyType &key) {
            iterator res(this);
            SNode *k = _find(key);
            if (k) res._pos = k;
            return res;
        }
        
        const_iterator find(const KeyType &key) const {
			const_iterator res(this);
            SNode *k = _find(key);
            if (k) res._pos = k;
            return res;
        }
    };
}
