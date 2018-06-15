#pragma once

#include <cstdio>

#include "lib/algorithm.hpp"
#include "lib/b_plus_tree.hpp"
#include "lib/utility.hpp"
#include "lib/vector.hpp"

#include "ticket.hpp"

namespace sjtu
{
    class Station
    {
    public:
        int name;
        Time arriveTime;
        Time startTime;
        String currency[5];
        double price[5];

        Station() = default;

        Station(int Name, const char *arrive, const char *start, String *Curr, double *Price):
            name(Name), arriveTime(arrive), startTime(start)
        {
            for (int i = 0; i < 5; ++i)
            {
                price[i] = Price[i];
                currency[i] = Curr[i];
            }
        }

        Station(const Station &oth)
        {
            name = oth.name;
            arriveTime = oth.arriveTime;
            startTime = oth.startTime;
            for (int i = 0; i < 5; ++i)
            {
                price[i] = oth.price[i];
                currency[i] = oth.currency[i];
            }
        }

        Station &operator = (const Station &oth)
        {
            if (this == &oth) return *this;
            name = oth.name;
            arriveTime = oth.arriveTime;
            startTime = oth.startTime;
            for (int i = 0; i < 5; ++i)
            {
                price[i] = oth.price[i];
                currency[i] = oth.currency[i];
            }
            return *this;
        }
    };

    class Train
    {
    public:
        String id;
        String name;
        char catalog;
        int stationCnt;
        int ticketKindCnt;
        String tickets[5];
        Station stations[60];
        int status;

        Train()
        {
            status = 0;
        }

        Train(const String &_tid, const String &_name, char _catalog, int _stationCnt, int _ticKindCnt, String *_tickets, Station *_stations):
            id(_tid), name(_name), catalog(_catalog), stationCnt(_stationCnt), ticketKindCnt(_ticKindCnt)
        {
            for (int i = 0; i < ticketKindCnt; ++i)
                tickets[i] = _tickets[i];
            for (int i = 0; i < stationCnt; ++i)
                stations[i] = _stations[i];
            status = 0;
        }
    };

    class Trains
    {
        BPTree<String, Train> T;
        OrderTime ticketCnts;

    public:
        Tickets tickets;

    public:
        Trains(): T("data_trains.rwdb") {}

        ~Trains() = default;

        bool Insert(const String &tid, const String &name, char catalog, int stationCnt, int ticKindCnt, String *tickets, Station *stations)
        {
            if (T.query(tid).second == true) return false;
            T.insert(tid, Train(tid, name, catalog, stationCnt, ticKindCnt, tickets, stations));
            return true;
        }

        bool Sale(const String &tid)
        {
            auto t = T.query(tid);
            if (t.second == false || t.first.status == 2) return false;

            for (int i = 0; i < t.first.stationCnt; ++i)
                for (int j = i + 1; j < t.first.stationCnt; ++j)
                    tickets.Insert(t.first.stations[i].name, t.first.stations[j].name, t.first.catalog, tid);

            t.first.status = 1;
            T.modify(tid, t.first);

            return true;
        }

        pair<Train, bool> Query(const String &tid)
        {
            return T.query(tid);
        }

        bool Delete(const String &tid)
        {
            auto t = T.query(tid);
            if (t.second == false || t.first.status == 1) return false;
            T.erase(tid);
            return true;
        }

        bool Modify(const String &tid, const String &name, char catalog, int stationCnt, int ticKindCnt, String *tickets, Station *stations)
        {
            auto t = T.query(tid);
            if (t.second == false || t.first.status == 1) return false;
            T.modify(tid, Train(tid, name, catalog, stationCnt, ticKindCnt, tickets, stations));
            return true;
        }

        void Clear()
        {
            T.clear();
            tickets.Clear();
            ticketCnts.Clear();
        }
    };
}
