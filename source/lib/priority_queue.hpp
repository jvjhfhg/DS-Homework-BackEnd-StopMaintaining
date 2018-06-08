#pragma once

#include <cstddef>
#include <functional>
#include "exceptions.hpp"

#include "algorithm.hpp"

namespace sjtu {
    template <class Type, class Compare = std::less<Type>>
    class priority_queue {
        struct Node {
            Node *ch[2];
            size_t _len;
            Type val;
            
            Node(const Type &obj): val(obj) {
                ch[0] = ch[1] = nullptr;
                _len = 0;
            }
            
            ~Node() = default;
        };
        
        bool _compare(const Type &a, const Type &b) {
            static Compare cmp;
            return cmp(a, b);
        }
        
      private:
        Node *root;
        size_t _size;
        
      private:
        Node *_merge(Node *p, Node *q) {
            if (p == nullptr) return q;
            if (q == nullptr) return p;
            if (_compare(p->val, q->val)) swap(p, q);
            p->ch[1] = _merge(p->ch[1], q);
            if (p->ch[0] == nullptr || (p->ch[1] != nullptr && p->ch[1]->_len > p->ch[0]->_len))
                swap(p->ch[0], p->ch[1]);
            p->_len = p->ch[1] != nullptr ? p->ch[1]->_len + 1 : 0;
            return p;
        }
        
        void _insert(Node *&k, const Type &obj) {
            Node *tmp = new Node(obj);
            k = _merge(k, tmp);
        }
        
        Node *_copy(Node *k) {
            if (k == nullptr) return nullptr;
            Node *p = new Node(k->val);
            p->_len = k->_len;
            for (size_t i = 0; i < 2; ++i)
                p->ch[i] = _copy(k->ch[i]);
            return p;
        }
        
        void _die(Node *k) {
            if (k == nullptr) return;
            for (size_t i = 0; i < 2; ++i)
                _die(k->ch[i]);
            delete k;
        }
        
      public:
        priority_queue() {
            root = nullptr;
            _size = 0;
        }
        
        priority_queue(const priority_queue &oth) {
            _size = oth._size;
            root = _copy(oth.root);
        }
        
        priority_queue &operator = (const priority_queue &oth) {
            if (this == &oth) return *this;
            _size = oth._size;
            _die(root);
            root = _copy(oth.root);
            return *this;
        }
        
        ~priority_queue() {
            _die(root);
        }
        
        const Type top() const {
            if (_size == 0)
                throw container_is_empty();
            return root->val;
        }
        
        void push(const Type &obj) {
            _insert(root, obj);
            ++_size;
        }
        
        void pop() {
            if (_size == 0)
                throw container_is_empty();
            Node *tmp = root;
            root = _merge(root->ch[0], root->ch[1]);
            --_size;
            delete tmp;
        }
        
        size_t size() const {
            return _size;
        }
        
        bool empty() const {
            return _size == 0;
        }
        
        void merge(priority_queue &oth) {
            root = _merge(root, oth.root);
            oth.root = nullptr;
            
            _size += oth._size;
            oth._size = 0;
        }
    };
}

#endif
