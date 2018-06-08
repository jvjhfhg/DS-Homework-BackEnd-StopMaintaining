#pragma once

#include <cstring>

namespace sjtu {
    template <class Type>
    void swap(Type &a, Type &b) {
        Type t(a); a = b; b = t;
    }
    
    class String {
    private:
        char _str[45];
        int _length;

    public:
        String() = default;

        String(const char *str) {
            _length = strlen(_str);
            for (int i = 0; i < _length; ++i)
                _str[i] = str[i];
            _str[_length] = '\0';
        }

        String(const String &oth) {
            _length = oth._length;
            for (int i = 0; i < _length; ++i)
                _str[i] = oth._str[i];
            _str[_length] = '\0';
        }

        String &operator = (const String &oth) {
            if (this == &oth) return *this;
            _length = oth._length;
            for (int i = 0; i < _length; ++i)
                _str[i] = oth._str[i];
            _str[_length] = '\0';
            return *this;
        }

        bool operator < (const String &oth) const {
            for (int i = 0; i < _length && i < oth._length; ++i) {
                if (_str[i] < oth._str[i]) return true;
                if (_str[i] > oth._str[i]) return false;
            }
            return _length < oth._length;
        }

        bool operator > (const String &oth) const {
            return oth < *this;
        }

        bool operator == (const String &oth) const {
            if (_length != oth._length) return false;
            for (int i = 0; i < _length; ++i)
                if (_str[i] != oth._str[i]) return false;
            return true;
        }

        const int Length() const {
            return _length;
        }

        const char *Str() const {
            return _str;
        }
    };

    struct Time {
        int hour, minute;

        Time(int h = 0, int m = 0): hour(h), minute(m) {}

        Time(const char *s) {
            sscanf(s, "%d:%d", &hour, &minute);
        }

        bool operator < (const Time &oth) const {
            return hour == oth.hour ? minute < oth.minute : hour < oth.hour;
        }

        const char *ToString() const {
            static char res[6];
            sprintf(res, "%02d:%02d", hour, minute);
            return res;
        }
    };

    struct Date {
        int year, month, day;

        Date(int y = 0, int m = 0, int d = 0): year(y), month(m), day(d) {}

        Date(const char *s) {
            sscanf(s, "%d-%d-%d", &year, &month, &day);
        }

        bool operator < (const Date &oth) const {
            return year == oth.year ? (month == oth.month ? day < oth.day : month < oth.month) : year < oth.year;
        }

        const char *ToString() {
            static char res[11];
            sprintf(res, "%04d-%02d-%02d", year, month, day);
            return res;
        }
    };
}
