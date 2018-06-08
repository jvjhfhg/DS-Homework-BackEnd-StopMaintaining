#pragma once

#include <cstdio>

#include "lib/algorithm.hpp"
#include "lib/fake_b_plus_tree.hpp"
#include "lib/utility.hpp"
#include "lib/vector.hpp"

namespace sjtu {
    class Station {
    public:
        String name;
        Time arriveTime;
        Time startTime;
        String currency;
        double price;

        Station() = default;

        Station(const char *_name, const char *_arrive, const char *_start, const char *_curr, double _price):
            name(_name), arriveTime(_arrive), startTime(_start), currency(_curr), price(_price) {}
        
        Station(const Station &oth) {
            name = oth.name;
            arriveTime = oth.arriveTime;
            startTime = oth.startTime;
            currency = oth.currency;
            price = oth.price;
        }
        
        Station &operator = (const Station &oth) {
            if (*this == oth) return *this;
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
    };

    class Trains {
    private:
        BPTree<String, Train> T;

    public:
        Trains(): T("data_trains") {}

        ~Trains() = default;

        bool AddTrain(const char *tid, const char *name, char catalog, int stationCnt, int ticKind) {
            Train t;
            if (T.query(t.id).second) return false;
            T.insert(t.id, t);
            return true;
        }


    };
}
