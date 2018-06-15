#pragma once

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "lib/algorithm.hpp"
#include "lib/b_plus_tree.hpp"
#include "lib/utility.hpp"
#include "lib/vector.hpp"

namespace sjtu
{
    class Tickets
    {
        struct KeyData
        {
            int loc1, loc2;
            char catalog;

            KeyData() = default;

            KeyData(int a, int b, char c):
                loc1(a), loc2(b), catalog(c) {}

            bool operator < (const KeyData &oth) const
            {
                return loc1 == oth.loc1 ? (loc2 == oth.loc2 ? catalog < oth.catalog : loc2 < oth.loc2) : loc1 < oth.loc1;
            }

            bool operator == (const KeyData &oth) const
            {
                return loc1 == oth.loc1 && loc2 == oth.loc2 && catalog == oth.catalog;
            }
        };

        BPTree<KeyData, String> T;

    public:
        Tickets(): T("data_tickets.rwdb") {}

        pair<vector<String>, bool> Query(int loc1, int loc2, char catalog)
        {
            auto t = T.query(KeyData(loc1, loc2, catalog));
            if (t.second == false) return make_pair(vector<String>(), false);

            // BPTree<String, bool> T2(t.first.Str());
            // return make_pair(T2.traverse(), true);

            vector<String> res;
            std::fstream datas(t.first.Str(), std::fstream::in | std::fstream::binary);
            String tmp;
            while (datas.read((char *)(&tmp), sizeof tmp))
            {
                res.push_back(tmp);
            }
            datas.close();
            return make_pair(res, true);
        }

        void Insert(int loc1, int loc2, char catalog, const String &tid)
        {
            KeyData key(loc1, loc2, catalog);
            auto t = T.query(key);
            String file;
            if (t.second == false)
            {
                static char tmp[45];
                sprintf(tmp, "data_tickets_%d_%d_%c.rwdb", loc1, loc2, catalog);
                file = tmp;
                T.insert(key, file);
            }
            else
            {
                file = t.first;
            }

            // BPTree<String, bool> T2(file.Str());
            // T2.insert(tid, true);

            std::fstream datas(file.Str(), std::fstream::out | std::fstream::app | std::fstream::binary);
            datas.write((const char *)(&tid), sizeof(String));
            datas.close();
        }

        void Clear()
        {
            static std::fstream datas;
            auto vec = T.traverse();
            for (int i = 0; i < (int)vec.size(); ++i)
            {
                datas.open(vec[i].second.Str(), std::fstream::out);
                datas.close();
            }
            T.clear();
        }
    };

    class OrderTime
    {
        struct KeyData
        {
            String tid;
            int ticketKind;
            int stationIdx;
            Date date;

            KeyData() = default;

            KeyData(const String &_tid, int _ticKind, int _stationIdx, Date _date):
                tid(_tid), ticketKind(_ticKind), stationIdx(_stationIdx), date(_date) {}

            bool operator < (const KeyData &oth) const
            {
                if (tid != oth.tid) return tid < oth.tid;
                if (ticketKind != oth.ticketKind) return ticketKind < oth.ticketKind;
                if (stationIdx != oth.stationIdx) return stationIdx < oth.stationIdx;
                return date < oth.date;
            }

            bool operator == (const KeyData &oth) const
            {
                return tid == oth.tid && ticketKind == oth.ticketKind && stationIdx == oth.stationIdx && date == oth.date;
            }
        };

        BPTree<KeyData, int> T;

    public:
        OrderTime(): T("data_order_time.rwdb") {}

        void Add(const String &tid, int ticKind, int stationIdx, const Date &date, int ticCnt)
        {
            KeyData key(tid, ticKind, stationIdx, date);
            auto t = T.query(key);
            int cnt = t.first;
            if (t.second == false)
            {
                cnt = 0;
                T.insert(key, cnt);
            }
            cnt += ticCnt;
            T.modify(key, cnt);
        }

        int Query(const String &tid, int ticKind, int stationIdx, const Date &date)
        {
            auto t = T.query(KeyData(tid, ticKind, stationIdx, date));
            return t.second == false ? 0 : t.first;
        }

        void Clear()
        {
            T.clear();
        }
    };

    class OrderUser
    {
        struct KeyData
        {
            int id;
            Date date;
            char catalog;

            KeyData() = default;

            KeyData(int _id, const Date &_date, char _catalog):
                id(_id), date(_date), catalog(_catalog) {}

            bool operator < (const KeyData &oth) const
            {
                return id == oth.id ? (date == oth.date ? catalog < oth.catalog : date < oth.date) : id < oth.id;
            }

            bool operator == (const KeyData &oth) const
            {
                return id == oth.id && date == oth.date && catalog == oth.catalog;
            }
        };

        struct Key2
        {
            String tid;
            int loc1, loc2;

            Key2() = default;

            Key2(const String &_tid, int _l1, int _l2): tid(_tid), loc1(_l1), loc2(_l2) {}

            bool operator < (const Key2 &oth) const
            {
                return tid == oth.tid ? (loc1 == oth.loc1 ? loc2 < oth.loc2 : loc1 < oth.loc1) : tid < oth.tid;
            }

            bool operator == (const Key2 &oth) const
            {
                return tid == oth.tid && loc1 == oth.loc1 && loc2 == oth.loc2;
            }
        };

        BPTree<KeyData, String> T;

    public:
        struct Order
        {
            int loc1;
            Date date1;
            Time time1;
            int loc2;
            Date date2;
            Time time2;
            int cnt[5];

            Order() = default;

            Order(int _l1, const Date &_d1, const Time &_t1, int _l2, const Date &_d2, const Time &_t2, int *_cnt):
                loc1(_l1), date1(_d1), time1(_t1), loc2(_l2), date2(_d2), time2(_t2)
            {
                for (int i = 0; i < 5; ++i)
                    cnt[i] = _cnt[i];
            }
        };

    public:
        OrderUser(): T("data_order_user.rwdb") {}

        vector<pair<String, Order>> Query(int id, const Date &date, char catalog)
        {
            auto t = T.query(KeyData(id, date, catalog));
            if (t.second == false) return vector<pair<String, Order>>();
            vector<pair<String, Order>> res;

            BPTree<Key2, Order> T2(t.first.Str());
            auto vec = T2.traverse();
            for (int i = 0; i < (int)vec.size(); ++i)
            {
                res.push_back(make_pair(vec[i].first.tid, vec[i].second));
            }
            return res;
        }

        const int *QueryActualTicket(int id, const String &tid, const Date &date, char catalog, int l1, int l2)
        {
            static int res[5];
            memset(res, 0, sizeof res);
            auto t = T.query(KeyData(id, date, catalog));
            if (t.second == false) return res;

            BPTree<Key2, Order> T2(t.first.Str());
            auto order = T2.query(Key2(tid, l1, l2));
            if (order.second == false) return res;
            for (int i = 0; i < 5; ++i)
                res[i] = order.first.cnt[i];
            return res;
        }

        void Add(int id, const String &tid, char catalog, int l1, const Date &d1, const Time &t1, int l2, const Date &d2, const Time &t2, int *cnt)
        {
            auto t = T.query(KeyData(id, d1, catalog));
            String file;
            if (t.second == false)
            {
                static char tmp[45];
                sprintf(tmp, "data_orders_%d_%s_%c.rwdb", id, d1.ToString(), catalog);
                file = tmp;
                T.insert(KeyData(id, d1, catalog), file);
            }
            else
            {
                file = t.first;
            }

            BPTree<Key2, Order> T2(file.Str());
            auto order = T2.query(Key2(tid, l1, l2));
            if (order.second == false)
                T2.insert(Key2(tid, l1, l2), Order(l1, d1, t1, l2, d2, t2, cnt));
            else
            {
                for (int i = 0; i < 5; ++i)
                    order.first.cnt[i] += cnt[i];
                T2.modify(Key2(tid, l1, l2), order.first);
            }
        }

        void Clear()
        {
            auto vec = T.traverse();
            for (int i = 0; i < (int)vec.size(); ++i)
            {
                BPTree<Key2, Order> T2(vec[i].second.Str());
                T2.clear();
            }
            T.clear();
        }
    };
}
