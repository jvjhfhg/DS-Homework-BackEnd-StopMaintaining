#pragma once

#include <cstdio>

#include "lib/algorithm.hpp"
#include "lib/fake_b_plus_tree.hpp"
#include "lib/utility.hpp"
#include "lib/vector.hpp"

namespace sjtu {
    class Tickets {
        struct KeyData {
            int loc1, loc2;
            char catalog;
            
            KeyData() = default;
            
            KeyData(int a, int b, char c): 
                loc1(a), loc2(b), catalog(c) {}
            
            bool operator < (const KeyData &oth) const {
                return loc1 == oth.loc1 ? (loc2 == oth.loc2 ? catalog < oth.catalog : loc2 < oth.loc2) : loc1 < oth.loc1;
            }
            
            bool operator == (const KeyData &oth) const {
                return loc1 == oth.loc1 && loc2 == oth.loc2 && catalog == oth.catalog;
            }
        };
        
        BPTree<KeyData, String> T;
        
    public:
        Tickets(): T("data/data_tickets") {}
        
        pair<vector<pair<String, bool>>, bool> Query(int loc1, int loc2, char catalog) {
            auto t = T.query(KeyData(loc1, loc2, catalog));
            if (t.second == false) return make_pair(vector<pair<String, bool>>(), false);
            
            BPTree<String, bool> T2(t.first.Str());
            return make_pair(T2.traverse(), true);
        }
        
        void Insert(int loc1, int loc2, char catalog, const String &tid) {
            KeyData key(loc1, loc2, catalog);
            auto t = T.query(key);
            String file;
            if (t.second == false) {
                static char tmp[45];
                sprintf(tmp, "data/%04d_%04d_%c", loc1, loc2, catalog);
                file = tmp;
                T.insert(key, file);
            } else {
                file = t.first;
            }
            
            BPTree<String, bool> T2(file.Str());
            T2.insert(tid, true);
        }
        
        void Clear() {
            T.clear();
        }
    };
    
    class OrderTime {
        struct KeyData {
            String tid;
            int ticketKind;
            int stationIdx;
            Date date;
            
            KeyData() = default;
            
            KeyData(const String &_tid, int _ticKind, int _stationIdx, Date _date): 
                tid(_tid), ticketKind(_ticKind), stationIdx(_stationIdx), date(_date) {}
            
            bool operator < (const KeyData &oth) const {
                if (tid != oth.tid) return tid < oth.tid;
                if (ticketKind != oth.ticketKind) return ticketKind < oth.ticketKind;
                if (stationIdx != oth.stationIdx) return stationIdx < oth.stationIdx;
                return date < oth.date;
            }
            
            bool operator == (const KeyData &oth) const {
                return tid == oth.tid && ticketKind == oth.ticketKind && stationIdx == oth.stationIdx && date == oth.date;
            }
        };
        
        BPTree<KeyData, int> T;
        
    public:
        OrderTime(): T("data/order_time") {}
        
        void Add(const String &tid, int ticKind, int stationIdx, Date date, int ticCnt) {
            KeyData key(tid, ticKind, stationIdx, date);
            auto t = T.query(key);
            int cnt = t.first;
            if (t.second == false) {
                cnt = 0;
                T.insert(key, cnt);
            }
            cnt += ticCnt;
            T.modify(key, cnt);
        }
        
        int Query(const String &tid, int ticKind, int stationIdx, Date date) {
            auto t = T.query(KeyData(tid, ticKind, stationIdx, date));
            return t.second == false ? 0 : t.first;
        }
        
        void Clear() {
            T.clear();
        }
    };
    
    class OrderUser {
        
    };
}
