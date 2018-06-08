#pragma once

#include <cstdio>

#include "lib/algorithm.hpp"
#include "lib/b_plus_tree.hpp"
#include "lib/utility.hpp"
#include "lib/vector.hpp"

namespace sjtu {
    class Station {
    public:
        String name;
        Time arrive;
        Time start;

    };

    class Train {
        enum Status {
            Private = 0, Public = 1
        };

    public:
        String id;
        String name;
        char catalog;
        int stationCnt;
        int priceCnt;
        String priceName[5];
        Station stations[60];
    };
}