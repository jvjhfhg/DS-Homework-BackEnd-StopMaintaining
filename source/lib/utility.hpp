#pragma once

namespace sjtu {
    template <class T1, class T2>
    class pair {
    public:
        T1 first;
        T2 second;
        
    public:
        constexpr pair(): first(), second() {}

        pair(const pair &oth) = default;

        pair(pair &&oth) = default;

        pair(const T1 &x, const T2 &y): first(x), second(y) {}

        template <class U1, class U2>
        pair(U1 &&x, U2 &&y): first(x), second(y) {}

        template<class U1, class U2>
        pair(const pair<U1, U2> &oth): first(oth.first), second(oth.second) {}

        template<class U1, class U2>
        pair(pair<U1, U2> &&oth): first(oth.first), second(oth.second) {}

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
    };
}
