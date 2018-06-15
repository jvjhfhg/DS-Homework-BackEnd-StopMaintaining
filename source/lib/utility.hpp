#pragma once

#include <utility>

namespace sjtu {
    template <class T1, class T2>
    class pair {
    public:
        T1 first;
        T2 second;
        
    public:
        pair(): first(), second() {}

        pair(const pair &oth) = default;

        pair &operator = (const pair &oth) {
            if (this == &oth) return *this;
            first = oth.first;
            second = oth.second;
            return *this;
        }
        
        template <class U1, class U2>
        pair &operator = (const pair<U1, U2> &oth) {
            if (this == &oth) return *this;
            first = oth.first;
            second = oth.second;
            return *this;
        }

        pair(const T1 &x, const T2 &y): first(x), second(y) {}

        template <class U1, class U2>
        pair(const U1 &x, const U2 &y): first(x), second(y) {}

        template<class U1, class U2>
        pair(const pair<U1, U2> &oth): first(oth.first), second(oth.second) {}

        friend bool operator < (const pair &x, const pair &y) {
            return x.first == y.first ? x.second < y.second : x.first < y.first;
        }
        
        friend bool operator > (const pair &x, const pair &y) {
            return y < x;
        }
        
        friend bool operator <= (const pair &x, const pair &y) {
            return !(y < x);
        }
        
        friend bool operator >= (const pair &x, const pair &y) {
            return !(x < y);
        }

        friend bool operator == (const pair &x, const pair &y) {
            return x.first == y.first && x.second == y.second;
        }
        
        friend bool operator != (const pair &x, const pair &y) {
            return !(x == y);
        }

        operator std::pair<T1, T2>() const {
            return std::make_pair(first, second);
        }
    };
    
    template <class T1, class T2>
    pair<T1, T2> make_pair(const T1 &first, const T2 &second) {
        return pair<T1, T2>(first, second);
    }
}
