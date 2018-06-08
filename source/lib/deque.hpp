#pragma once

#include "exceptions.hpp"

#include <cstddef>

namespace sjtu {
    template <class Type>
    class deque {
      private:
        static const size_t BlockSize = 500;
        
        struct BlockNode;
        struct LinkNode;
        
        typedef BlockNode BNode;
        typedef LinkNode LNode;
        
        LNode *_head_, *_tail_;
        size_t _size_;
        
        struct BlockNode {
            BNode *_pre, *_nxt;
            LNode *_block;
            Type _val;
            
            BlockNode(const Type &val): _val(val) {
                _pre = _nxt = nullptr;
                _block = nullptr;
            }
            
            static void _Copy(BNode *&ths, BNode *pre, BNode *oth, LNode *_b) {
                if (!oth) return;
                
                ths = new BNode(oth->_val);
                ths->_pre = pre;
                _Copy(ths->_nxt, ths, oth->_nxt, _b);
                ths->_block = _b;
            }
            
            void _Delete() {
                if (!this) return;
                
                _nxt->_Delete();
                delete this;
            }
        };
        
        struct LinkNode {
            LNode *_pre, *_nxt;
            BNode *_head, *_tail;
            size_t _size; // 0-base, empty nodes exist
            
            LinkNode() {
                _pre = _nxt = nullptr;
                _head = _tail = nullptr;
                _size = 0;
            }
            
            static void _Copy(LNode *&ths, LNode *pre, LNode *oth) {
                if (!oth) return;
                
                ths = new LNode();
                _Copy(ths->_nxt, ths, oth->_nxt);
                ths->_pre = pre;
                ths->_size = oth->_size;
                BNode::_Copy(ths->_head, nullptr, oth->_head, ths);
                ths->_tail = ths->_head;
                for (size_t i = 1; i < ths->_size; ++i)
                    ths->_tail = ths->_tail->_nxt;
            }
            
            void _Delete() {
                if (!this) return;
                
                _nxt->_Delete();
                _head->_Delete();
                delete this;
            }
            
            static BNode *_Find(LNode *k, size_t idx, bool _sys = false) {
                while (k && idx >= k->_size) {
                    idx -= k->_size;
                    k = k->_nxt;
                }
                if (!k) {
                    if (_sys) return nullptr;
                    throw index_out_of_bound();
                }
                BNode *p = k->_head;
                for (size_t i = 0; i < idx; ++i) {
                    p = p->_nxt;
                }
                return p;
            }
        };
        
        void _MergeToNext(LNode *ths) {
            LNode *nx = ths->_nxt;
            
            if (!ths->_size) {
                nx->_pre = ths->_pre;
                if (ths->_pre) ths->_pre->_nxt = nx;
                
                if (ths == _head_) _head_ = nx;
                
                delete ths;
                
                return;
            }
            
            if (!nx->_size) {
                ths->_nxt = nx->_nxt;
                if (nx->_nxt) nx->_nxt->_pre = ths;
                
                if (nx == _tail_) _tail_ = ths;
                
                delete nx;
                
                return;
            }
            
            for (BNode *p = nx->_head; p; p = p->_nxt) {
                p->_block = ths;
            }
            ths->_tail->_nxt = nx->_head;
            nx->_head->_pre = ths->_tail;
            ths->_tail = nx->_tail;
            ths->_size += nx->_size;
            
            ths->_nxt = nx->_nxt;
            if (nx->_nxt) nx->_nxt->_pre = ths;
            
            if (nx == _tail_) _tail_ = ths;
            
            delete nx;
        }
        
        void _SplitFrom(LNode *ths, size_t idx) { // _size >= 2
            LNode *nx = new LNode();
            
            nx->_nxt = ths->_nxt;
            if (ths->_nxt) ths->_nxt->_pre = nx;
            nx->_pre = ths;
            ths->_nxt = nx;
            
            nx->_tail = ths->_tail;
            BNode *p = ths->_head;
            for (size_t i = 1; i < idx; ++i)
                p = p->_nxt;
            ths->_tail = p;
            nx->_head = p->_nxt;
            ths->_tail->_nxt = nx->_head->_pre = nullptr;
            nx->_size = ths->_size - idx; ths->_size = idx;
            
            for (p = nx->_head; p; p = p->_nxt)
                p->_block = nx;
            
            if (ths == _tail_) _tail_ = nx;
        }
        
        BNode *_Insert(size_t idx, const Type &val, BNode *pos = nullptr) {
            BNode *p = new BNode(val);
            if (idx == 0) { // push_front()
                p->_block = _head_;
                p->_nxt = _head_->_head;
                if (_head_->_head) _head_->_head->_pre = p;
                
                _head_->_head = p;
                ++_head_->_size;
                if (_head_->_size == 1) _head_->_tail = _head_->_head;
                if (_head_->_size >= BlockSize * 2)
                    _SplitFrom(_head_, _head_->_size / 2);
            } else if (idx == _size_) { // push_back()
                p->_block = _tail_;
                p->_pre = _tail_->_tail;
                if (_tail_->_tail) _tail_->_tail->_nxt = p;
                
                _tail_->_tail = p;
                ++_tail_->_size;
                if (_tail_->_size == 1) _tail_->_head == _tail_->_tail;
                if (_tail_->_size >= BlockSize * 2)
                    _SplitFrom(_tail_, _tail_->_size / 2);
            } else {
                LNode *b = pos->_block;
                
                p->_block = b;
                p->_pre = pos->_pre;
                if (pos->_pre) pos->_pre->_nxt = p;
                p->_nxt = pos;
                pos->_pre = p;
                
                if (pos == b->_head) b->_head = p;
                ++b->_size;
                if (b->_size >= BlockSize * 2)
                    _SplitFrom(b, b->_size / 2);
            }
            ++_size_;
            return p;
        }
        
        BNode *_Erase(size_t idx, BNode *pos = nullptr) {
            BNode *p = nullptr;
            if (idx == 0) { // pop_front()
                pos = _head_->_head;
                
                _head_->_head = pos->_nxt;
                if (_head_->_head) _head_->_head->_pre = nullptr;
                
                --_head_->_size;
                if (!_head_->_size) _head_->_tail = nullptr;
                if (_head_->_nxt && _head_->_size + _head_->_nxt->_size < BlockSize * 2)
                    _MergeToNext(_head_);
                
                delete pos;
                p = _head_->_head;
            } else if (idx == _size_ - 1) { // pop_back()
                pos = _tail_->_tail;
                
                _tail_->_tail = pos->_pre;
                if (_tail_->_tail) _tail_->_tail->_nxt = nullptr;
                
                --_tail_->_size;
                if (!_tail_->_size) _tail_->_head = nullptr;
                if (_tail_->_pre && _tail_->_pre->_size + _tail_->_size < BlockSize * 2)
                    _MergeToNext(_tail_->_pre);
                
                delete pos;
                p = nullptr;
            } else {
                LNode *b = pos->_block;
                
                if (pos->_pre) pos->_pre->_nxt = pos->_nxt;
                if (pos->_nxt) pos->_nxt->_pre = pos->_pre;
                
                if (pos == b->_head) b->_head = pos->_nxt;
                if (pos == b->_tail) b->_tail = pos->_pre;
                
                --b->_size;
                if (!b->_size) { // Delete the block
                    if (b == _head_ && b == _tail_) goto FunctionEnd;
                    
                    b->_head = b->_tail = nullptr;
                    
                    if (b->_pre) b->_pre->_nxt = b->_nxt;
                    if (b->_nxt) b->_nxt->_pre = b->_pre;
                    
                    if (b == _head_) _head_ = b->_nxt;
                    if (b == _tail_) _tail_ = b->_pre;
                    
                    p = b->_nxt ? b->_nxt->_head : nullptr;
                    delete b;
                } else {
                    if (pos->_nxt) p = pos->_nxt;
                    else p = b->_nxt ? b->_nxt->_head : nullptr;
                    
                    if (b->_nxt && b->_size + b->_nxt->_size < BlockSize * 2)
                        _MergeToNext(b);
                }
                delete pos;
            }
            FunctionEnd:
            --_size_;
            return p;
        }
        
      public:
        class const_iterator;
        class iterator {
            friend deque;
            
          private:
            BNode *_pos;
            size_t _idx;
            deque *_container;
            
          public:
            iterator(const deque *container = nullptr): _container((deque *)container) {
                _pos = nullptr;
                _idx = 0;
            }
            
            iterator(const iterator &oth): 
              _pos(oth._pos), _container(oth._container), _idx(oth._idx) {}
            
            iterator operator + (const int &n) const {
                if (n == 0) return *this;
                
                iterator res;
                
                res._container = _container;
                res._idx = _idx + n;
                res._pos = LNode::_Find(res._container->_head_, res._idx, true);
                
                return res;
            }
            
            iterator operator - (const int &n) const {
                return *this + (-n);
            }
            
            int operator - (const iterator &oth) const {
                if (_container != oth._container)
                    throw invalid_iterator();
                return (int)_idx - oth._idx;
            }
            
            iterator &operator += (const int &n) {
                return *this = *this + n;
            }
            
            iterator &operator -= (const int &n) {
                return *this = *this + (-n);
            }
            
            iterator operator ++ (int) {
                iterator res(*this);
                *this += 1;
                return res;
            }
            
            iterator &operator ++ () {
                return *this += 1;
            }
            
            iterator operator -- (int) {
                iterator res(*this);
                *this -= 1;
                return res;
            }
            
            iterator &operator -- () {
                return *this -= 1;
            }
            
            Type &operator * () const {
                if (_pos == nullptr) throw invalid_iterator();
                return _pos->_val;
            }
            
            Type *operator -> () const noexcept {
                return &(_pos->_val);
            }
            
            bool operator == (const iterator &oth) const {
                return _container == oth._container && _idx == oth._idx;
            }
            
            bool operator == (const const_iterator &oth) const {
                return _container == oth._container && _idx == oth._idx;
            }
            
            bool operator != (const iterator &oth) const {
                return !(*this == oth);
            }
            
            bool operator != (const const_iterator &oth) const {
                return !(*this == oth);
            }
        };
        
        class const_iterator {
            friend deque;
            
          private:
            BNode *_pos;
            size_t _idx;
            deque *_container;
            
          public:
            const_iterator(const deque *container = nullptr): _container((deque *)container) {
                _pos = nullptr;
                _idx = 0;
            }
            
            const_iterator(const const_iterator &oth): 
              _pos(oth._pos), _container(oth._container), _idx(oth._idx) {}
            
            const_iterator(const iterator &oth): 
              _pos(oth._pos), _container(oth._container), _idx(oth._idx) {}
            
            const_iterator operator + (int n) const {
                if (n == 0) return *this;
                
                const_iterator res;
                
                res._container = _container;
                res._idx = _idx + n;
                res._pos = LNode::_Find(res._container->_head_, res._idx, true);
                
                return res;
            }
            
            const_iterator operator - (const int &n) const {
                return *this + (-n);
            }
            
            int operator - (const const_iterator &oth) const {
                if (_container != oth._container)
                    throw invalid_iterator();
                return (int)_idx - oth._idx;
            }
            
            const_iterator &operator += (const int &n) {
                return *this = *this + n;
            }
            
            const_iterator &operator -= (const int &n) {
                return *this = *this + (-n);
            }
            
            const_iterator operator ++ (int) {
                const_iterator res(*this);
                *this += 1;
                return res;
            }
            
            const_iterator &operator ++ () {
                return *this += 1;
            }
            
            const_iterator operator -- (int) {
                const_iterator res(*this);
                *this -= 1;
                return res;
            }
            
            const_iterator &operator -- () {
                return *this -= 1;
            }
            
            const Type &operator * () const {
                return _pos->_val;
            }
            
            const Type *operator -> () const noexcept {
                return &(_pos->_val);
            }
            
            bool operator == (const iterator &oth) const {
                return _container == oth._container && _idx == oth._idx;
            }
            
            bool operator == (const const_iterator &oth) const {
                return _container == oth._container && _idx == oth._idx;
            }
            
            bool operator != (const iterator &oth) const {
                return !(*this == oth);
            }
            
            bool operator != (const const_iterator &oth) const {
                return !(*this == oth);
            }
        };
        
        deque() {
            _tail_ = _head_ = new LNode();
            _size_ = 0;
        }
        
        deque(const deque &oth) {
            LNode::_Copy(_head_, nullptr, oth._head_);
            _tail_ = _head_;
            while (_tail_->_nxt) _tail_ = _tail_->_nxt;
            _size_ = oth._size_;
        }
        
        ~deque() {
            _head_->_Delete();
        }
        
        deque &operator = (const deque &oth) {
            if (&oth == this) return *this;
            _head_->_Delete();
            LNode::_Copy(_head_, nullptr, oth._head_);
            _tail_ = _head_;
            while (_tail_->_nxt) _tail_ = _tail_->_nxt;
            _size_ = oth._size_;
            return *this;
        }
        
        Type &at(size_t idx) {
            return LNode::_Find(_head_, idx)->_val;
        }
        
        const Type &at(size_t idx) const {
            return LNode::_Find(_head_, idx)->_val;
        }
        Type &operator [] (size_t idx) {
            return LNode::_Find(_head_, idx)->_val;
        }
        
        const Type operator [] (size_t idx) const {
            return LNode::_Find(_head_, idx)->_val;
        }
        
        const Type front() const {
            if (empty()) throw container_is_empty();
            return _head_->_head->_val;
        }
        
        const Type back() const {
            if (empty()) throw container_is_empty();
            return _tail_->_tail->_val;
        }
        
        iterator begin() {
            iterator res(this);
            
            res._pos = _head_->_head;
            res._idx = 0;
            
            return res;
        }
        
        const_iterator cbegin() const {
            const_iterator res(this);
            
            res._pos = _head_->_head;
            res._idx = 0;
            
            return res;
        }
        
        iterator end() {
            iterator res(this);
            
            res._pos = nullptr;
            res._idx = _size_;
            
            return res;
        }
        
        const_iterator cend() const {
            const_iterator res(this);
            
            res._pos = nullptr;
            res._idx = _size_;
            
            return res;
        }
        
        bool empty() const {
            return _size_ == 0;
        }
        
        size_t size() const {
            return _size_;
        }
        
        void clear() {
            _head_->_Delete();
            _tail_ = _head_ = new LNode();
            _size_ = 0;
        }
        
        iterator insert(iterator pos, const Type &value) {
            if (pos._container != this || pos._idx > _size_)
                throw invalid_iterator();
            
            iterator res(this);
            
            res._pos = _Insert(pos._idx, value, pos._pos);
            res._idx = pos._idx;
            
            return res;
        }
        
        iterator erase(iterator pos) {
            if (pos._container != this || pos._idx >= _size_)
                throw invalid_iterator();
            if (empty())
                throw container_is_empty();
            
            iterator res(this);
            
            res._pos = _Erase(pos._idx, pos._pos);
            res._idx = pos._idx;
            
            return res;
        }
        
        void push_back(const Type &value) {
            _Insert(_size_, value);
        }
        
        void pop_back() {
            if (empty())
                throw container_is_empty();
            _Erase(_size_ - 1);
        }
        
        void push_front(const Type &value) {
            _Insert(0, value);
        }
        
        void pop_front() {
            if (empty())
                throw container_is_empty();
            _Erase(0);
        }
    };
}
