#pragma once

#include <cstdio>

#include "lib/algorithm.hpp"
#include "lib/fake_b_plus_tree.hpp"
#include "lib/utility.hpp"
#include "lib/vector.hpp"

#include "Ticket.hpp"

namespace sjtu {
    class Station {
    public:
        int name;
        Time arriveTime;
        Time startTime;
        String currency;
        double price;

        Station() = default;

        Station(int _name, const char *_arrive, const char *_start, const char *_curr, double _price):
            name(_name), arriveTime(_arrive), startTime(_start), currency(_curr), price(_price) {}
        
        Station(const Station &oth) {
            name = oth.name;
            arriveTime = oth.arriveTime;
            startTime = oth.startTime;
            currency = oth.currency;
            price = oth.price;
        }
        
        Station &operator = (const Station &oth) {
            if (this == &oth) return *this;
            name = oth.name;
            arriveTime = oth.arriveTime;
            startTime = oth.startTime;
            currency = oth.currency;
            price = oth.price;
            return *this;
        }
    };

    class Train {
    public:
        enum Status {
            Private = 0, Public = 1
        };

        String id;
        String name;
        char catalog;
        int stationCnt;
        int ticketKindCnt;
        String tickets[5];
        Station stations[60];
        Status status;

        Train() {
            status = Status::Private;
        }
        
        Train(const String &_tid, const String &_name, char _catalog, int _stationCnt, int _ticKindCnt, String *_tickets, Station *_stations): 
            id(_tid), name(_name), catalog(_catalog), stationCnt(_stationCnt), ticketKindCnt(_ticKindCnt) {
            for (int i = 0; i < ticketKindCnt; ++i)
                tickets[i] = _tickets[i];
            for (int i = 0; i < stationCnt; ++i)
                stations[i] = _stations[i];
            status = Status::Private;
        }
    };

    class Trains {
    private:
        BPTree<String, Train> T;
        Tickets tickets;
        OrderTime ticketCnts;

    public:
        Trains(): T("data/data_trains") {}

        ~Trains() = default;

        bool Insert(const String &tid, const String &name, char catalog, int stationCnt, int ticKindCnt, String *tickets, Station *stations) {
            if (T.query(tid).second == true) return false;
            T.insert(tid, Train(tid, name, catalog, stationCnt, ticKindCnt, tickets, stations));
            return true;
        }

        bool Sale(const String &tid) {
            auto t = T.query(tid);
            if (t.second == false || t.first.status == Train::Status::Public) return false;
            
            for (int i = 0; i < t.first.stationCnt; ++i)
                for (int j = i + 1; j < t.first.stationCnt; ++j)
                    tickets.Insert(t.first.stations[i].name, t.first.stations[j].name, t.first.catalog, t.first.id);
            
            // TODO
            
            t.first.status = Train::Status::Public;
            T.modify(t.first.id, t.first);
            
            return true;
        }
        
        pair<Train, bool> Query(const String &tid) {
            return T.query(tid);
        }
        
        bool Delete(const String &tid) {
            auto t = T.query(tid);
            if (t.second == false) return false;
            T.erase(tid);
            return true;
        }
        
        bool Modify(const String &tid, const String &name, char catalog, int stationCnt, int ticKindCnt, String *tickets, Station *stations) {
            if (T.query(tid).second == false) return false;
            T.modify(tid, Train(tid, name, catalog, stationCnt, ticKindCnt, tickets, stations));
            return true;
        }
        
        void Clear() {
            T.clear();
            tickets.Clear();
            ticketCnts.Clear();
        }
    };
}
