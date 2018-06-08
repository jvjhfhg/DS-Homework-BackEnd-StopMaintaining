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
        String(const char *str = "") {
            _length = strlen(str);
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

        String &operator = (const char *str) {
            _length = strlen(str);
            for (int i = 0; i < _length; ++i)
                _str[i] = str[i];
            _str[_length] = '\0';
            return *this;
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

        bool operator != (const String &oth) const {
            return !(*this == oth);
        }

        int Length() const {
            return _length;
        }

        const char *Str() const {
            return _str;
        }

        void Read() {
            scanf("%s", _str);
            _length = strlen(_str);
        }
    };

    struct Time {
        int hour, minute;

        Time(int h = 0, int m = 0): hour(h), minute(m) {
            Stdize();
        }

        Time(const char *s) {
            sscanf(s, "%d:%d", &hour, &minute);
            Stdize();
        }

        bool operator < (const Time &oth) const {
            return hour == oth.hour ? minute < oth.minute : hour < oth.hour;
        }

        Time operator + (const Time &oth) const {
            Time res(hour + oth.hour, minute + oth.minute);
            res.Stdize();
            return res;
        }

        void Stdize() {
            hour = (hour + minute / 60) % 24;
            minute %= 60;
        }

        const char *ToString() const {
            static char res[6];
            sprintf(res, "%02d:%02d", hour, minute);
            return res;
        }
    };

    struct Date {
        static constexpr int days[2][13] = {{
            0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
        }, {
            0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
        }};

        int year, month, day;

        Date(int y = 0, int m = 0, int d = 0): year(y), month(m), day(d) {
            Stdize();
        }

        Date(const char *s) {
            sscanf(s, "%d-%d-%d", &year, &month, &day);
            Stdize();
        }

        Date operator + (int t) const {
            return Date(year, month, day + t);
        }

        bool operator < (const Date &oth) const {
            return year == oth.year ? (month == oth.month ? day < oth.day : month < oth.month) : year < oth.year;
        }

        bool IsLeap() const {
            return (year % 4 == 0 && year % 100 != 0) || year % 400 == 0;
        }

        void Stdize() {
            year += (month - 1) / 12;
            month = (month - 1) % 12 + 1;
            while (day > days[IsLeap()][month]) {
                day -= days[IsLeap()][month++];
                if (month == 13) {
                    ++year; month = 1;
                }
            }
        }

        const char *ToString() {
            static char res[11];
            sprintf(res, "%04d-%02d-%02d", year, month, day);
            return res;
        }
    };
}
