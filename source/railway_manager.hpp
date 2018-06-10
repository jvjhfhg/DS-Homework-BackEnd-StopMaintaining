#pragma once

#include <algorithm>

#include "lib/algorithm.hpp"
#include "lib/utility.hpp"
#include "lib/vector.hpp"

#include "User.hpp"
#include "Train.hpp"
#include "Ticket.hpp"
#include "Place.hpp"

namespace sjtu {
    Places places;
    
    Users users;
    Trains trains;

    OrderTime orderTime;
    OrderUser orderUser;

    class Interactor {
    public:
        /* 
         * User related
         */

        static int Register(const char *name, const char *password, const char *email, const char *phone) {
            return users.Register(name, password, email, phone);
        }

        static bool Login(int id, const char *password) {
            return users.Login(id, password);
        }

        // If second is false, the user doesn't exist. 
        static vector<String> QueryProfile(int id) {
            vector<String> res;
            auto t = users.Query(id);
            if (t.second == false)
                res.push_back("0");
            else {
                res.push_back(t.first.name);
                res.push_back(t.first.email);
                res.push_back(t.first.phone);
                res.push_back(String::Int((int)t.first.priv));
            }
            return res;
        }

        static bool ModifyProfile(int id, const char *name, const char *password, const char *email, const char *phone) {
            return users.Modify(id, name, password, email, phone);
        }

        static bool ModifyPrivilege(int id1, int id2, int priv) {
            return users.ModifyPrivilege(id1, id2, (User::Privilege)priv);
        }

        /* 
         * Ticket related
         */

        static vector<vector<String>> QueryTicket(const String &loc1, const String &loc2, const Date &date, char catalog) {
            int l1 = places.Query(loc1), l2 = places.Query(loc2);
            auto t = trains.tickets.Query(l1, l2, catalog);
            vector<vector<String>> res; 
            if (t.second == false) {
                return res; 
            }
            for (int i = 0; i < (int)t.first.size(); ++i) {
                String tid = t.first[i].first;
                Train train = trains.Query(tid).first;
                res.push_back(vector<String>());
                vector<String> &vec = res[res.size() - 1];
                vec.push_back(tid);
                for (int j = 0; j < train.stationCnt; ++j) if (train.stations[j].name == l1) {
                    vec.push_back(loc1);
                    vec.push_back(date.ToString());
                    vec.push_back(train.stations[j].startTime.ToString());

                    int deltaDay = 0;
                    for (int k = j + 1; k < train.stationCnt; ++k) {
                        if (train.stations[k].name == l2) {
                            if (train.stations[k].arriveTime < train.stations[k - 1].startTime) ++deltaDay;

                            vec.push_back(loc2);
                            vec.push_back((date + deltaDay).ToString());
                            vec.push_back(train.stations[k].arriveTime.ToString());

                            for (int tk = 0; tk < train.ticketKindCnt; ++tk) {
                                vec.push_back(train.tickets[tk]);

                                int ticketLeft = 2000;
                                double price = 0;
                                deltaDay = 0;
                                for (int sta = j + 1; sta <= k; ++sta) {
                                    if ((sta != k && train.stations[sta].startTime < train.stations[sta - 1].startTime) || 
                                        (sta == k && train.stations[sta].arriveTime < train.stations[sta - 1].startTime)) ++deltaDay;
                                    ticketLeft = std::min(ticketLeft, 2000 - orderTime.Query(tid, tk, sta, date + deltaDay));
                                    price += train.stations[sta].price;
                                }
                                
                                vec.push_back(String::Int(ticketLeft));
                                vec.push_back(train.stations[k].currency);
                                vec.push_back(String::Float(price));
                            }

                            break;
                        } else if (train.stations[k].startTime < train.stations[k - 1].startTime) ++deltaDay;
                    }

                    break;
                }
            }
            return res;
        }

        static pair<vector<String>, vector<String>> QueryTransfer(const String &loc1, const String &loc2, const Date &date, char catalog) {
            // TODO

            return pair<vector<String>, vector<String>>();
        }

        static bool BuyTicket(int id, int num, const String &tid, const String &loc1, const String &loc2, const Date &date, const String &ticketKind) {
            auto t = trains.Query(tid);
            if (t.second) return false;
            int l1 = places.Query(loc1), l2 = places.Query(loc2);
            Train train = t.first;
            int tk = -1;
            for (int i = 0; i < train.ticketKindCnt; ++i)
                if (ticketKind == train.tickets[i]) {
                    tk = i; break;
                }
            if (tk == -1) return false;
            for (int i = 0; i < train.stationCnt; ++i) if (train.stations[i].name == l1) {
                for (int j = i + 1; j < train.stationCnt; ++j) if (train.stations[j].name == l2) {
                    int ticketLeft = 2000, deltaDay = 0;
                    for (int sta = i + 1; sta <= j; ++sta) {
                        if ((sta != j && train.stations[sta].startTime < train.stations[sta - 1].startTime) ||
                            (sta == j && train.stations[sta].arriveTime < train.stations[sta - 1].startTime)) ++deltaDay;
                        ticketLeft = std::min(ticketLeft, 2000 - orderTime.Query(tid, tk, sta, date + deltaDay));
                    }
                    if (ticketLeft < num) return false;
                    deltaDay = 0;
                    for (int sta = i + 1; sta <= j; ++sta) {
                        if ((sta != j && train.stations[sta].startTime < train.stations[sta - 1].startTime) ||
                            (sta == j && train.stations[sta].arriveTime < train.stations[sta - 1].startTime)) ++deltaDay;
                        orderTime.Add(tid, tk, sta, date + deltaDay, num);
                    }

                    int tmp[] = {0, 0, 0, 0, 0};
                    tmp[tk] = num;

                    orderUser.Add(id, tid, train.catalog, l1, date, train.stations[i].startTime, l2, date + deltaDay, train.stations[j].arriveTime, tmp);

                    return true;
                }
                break;
            }
            return false;
        }

        static vector<vector<String>> QueryOrder(int id, const Date &date, char catalog) {
            auto t = orderUser.Query(id, date, catalog);
            vector<vector<String>> res;
            for (int i = 0; i < (int)t.size(); ++i) {
                res.push_back(vector<String>());
                vector<String> &vec = res[res.size() - 1];
                String tid = t[i].first;
                Train train = trains.Query(tid).first;
                vec.push_back(tid);
                vec.push_back(places.QueryName(t[i].second.loc1));
                vec.push_back(t[i].second.date1.ToString());
                vec.push_back(t[i].second.time1.ToString());
                vec.push_back(places.QueryName(t[i].second.loc2));
                vec.push_back(t[i].second.date2.ToString());
                vec.push_back(t[i].second.time2.ToString());
                for (int j = 0; j < train.ticketKindCnt; ++j) {
                    vec.push_back(train.tickets[j]);
                    vec.push_back(String::Int(t[i].second.cnt[j]));
                    for (int st1 = 0; st1 < train.stationCnt; ++st1) if (train.stations[st1].name == t[i].second.loc1) {
                        for (int st2 = st1 + 1; st2 < train.stationCnt; ++st2) if (train.stations[st2].name == t[i].second.loc2) {
                            double price = 0;
                            vec.push_back(train.stations[st2].currency);
                            for (int sta = st1 + 1; sta <= st2; ++sta)
                                price += train.stations[sta].price;
                            vec.push_back(String::Float(price));
                            break;
                        }
                        break;
                    }
                }
            }
            return res;
        }

        static bool RefundTicket(int id, int num, const String &tid, const String &loc1, const String &loc2, const Date &date, const String &ticketKind) {
            auto t = trains.Query(tid);
            if (t.second) return false;
            int l1 = places.Query(loc1), l2 = places.Query(loc2);
            Train train = t.first;
            int tk = -1;
            for (int i = 0; i < train.ticketKindCnt; ++i)
                if (ticketKind == train.tickets[i]) {
                    tk = i; break;
                }
            if (tk == -1) return false;
            for (int i = 0; i < train.stationCnt; ++i) if (train.stations[i].name == l1) {
                for (int j = i + 1; j < train.stationCnt; ++j) if (train.stations[j].name == l2) {
                    const int *ticketBought = orderUser.QueryActualTicket(id, tid, date, train.catalog, l1, l2);
                    if (ticketBought[tk] < num) return false;
                    int deltaDay = 0;
                    for (int sta = i + 1; sta <= j; ++sta) {
                        if ((sta != j && train.stations[sta].startTime < train.stations[sta - 1].startTime) ||
                            (sta == j && train.stations[sta].arriveTime < train.stations[sta - 1].startTime)) ++deltaDay;
                        orderTime.Add(tid, tk, sta, date + deltaDay, -num);
                    }

                    int tmp[] = {0, 0, 0, 0, 0};
                    tmp[tk] = -num;

                    orderUser.Add(id, tid, train.catalog, l1, date, train.stations[i].startTime, l2, date + deltaDay, train.stations[j].arriveTime, tmp);

                    return true;
                }
                break;
            }
            return false;
        }
        
        /* 
         * Train related
         */

        static bool AddTrain(vector<String> commands) {
            int idx = 0;
            String tid = commands[idx++];
            String name = commands[idx++];
            char catalog = *commands[idx++].Str();
            int stationCnt = commands[idx++].ToInt();
            int ticKindCnt = commands[idx++].ToInt();
            String tickets[5];
            for (int i = 0; i < 5; ++i)
                tickets[i] = commands[idx++];
            Station stations[60];
            for (int i = 0; i < stationCnt; ++i) {
                stations[i].name = places.Query(commands[idx++]);
                stations[i].arriveTime = (i == 0 ? Time("00:00") : Time(commands[idx].Str())); ++idx;
                stations[i].startTime = (i == stationCnt - 1 ? Time("00:00") : Time(commands[idx].Str())); ++idx;
                ++idx;
                stations[i].currency = commands[idx++];
                stations[i].price = commands[idx++].ToFloat();
            }
            return trains.Insert(tid, name, catalog, stationCnt, ticKindCnt, tickets, stations);
        }
        
        static bool SaleTrain(const String &tid) {
            return trains.Sale(tid);
        }
        
        static vector<String> QueryTrain(const String &tid) {
            vector<String> res;
            auto t = trains.Query(tid);
            if (t.second == false)
                res.push_back("0");
            else {
                res.push_back(t.first.id);
                res.push_back(t.first.name);
                res.push_back(&t.first.catalog);
                res.push_back(String::Int(t.first.stationCnt));
                res.push_back(String::Int(t.first.ticketKindCnt));
                for (int i = 0; i < t.first.ticketKindCnt; ++i)
                    res.push_back(t.first.tickets[i]);
                for (int i = 0; i < t.first.stationCnt; ++i) {
                    res.push_back(places.QueryName(t.first.stations[i].name));
                    if (i == 0) res.push_back("xx:xx");
                    else res.push_back(t.first.stations[i].arriveTime.ToString());
                    if (i == t.first.stationCnt - 1) res.push_back("xx:xx");
                    else res.push_back(t.first.stations[i].startTime.ToString());
                    if (i == 0 || i == t.first.stationCnt - 1) res.push_back("00:00");
                    else res.push_back((t.first.stations[i].startTime - t.first.stations[i].arriveTime).ToString());
                    res.push_back(t.first.stations[i].currency);
                    res.push_back(String::Float(t.first.stations[i].price));
                }
            }
            return res;
        }
        
        static bool DeleteTrain(const String &tid) {
            return trains.Delete(tid);
        }
        
        static bool ModifyTrain(vector<String> commands) {
            int idx = 0;
            String tid = commands[idx++];
            String name = commands[idx++];
            char catalog = *commands[idx++].Str();
            int stationCnt = commands[idx++].ToInt();
            int ticKindCnt = commands[idx++].ToInt();
            String tickets[5];
            for (int i = 0; i < 5; ++i)
                tickets[i] = commands[idx++];
            Station stations[60];
            for (int i = 0; i < stationCnt; ++i) {
                stations[i].name = places.Query(commands[idx++]);
                stations[i].arriveTime = (i == 0 ? Time("00:00") : Time(commands[idx].Str())); ++idx;
                stations[i].startTime = (i == stationCnt - 1 ? Time("00:00") : Time(commands[idx].Str())); ++idx;
                ++idx;
                stations[i].currency = commands[idx++];
                stations[i].price = commands[idx++].ToFloat();
            }
            return trains.Modify(tid, name, catalog, stationCnt, ticKindCnt, tickets, stations);
        }
        
        /* 
         * System related
         */

        static void Clean() {
            places.Clear();
            users.Clear();
            trains.Clear();
            orderTime.Clear();
            orderUser.Clear();
        }
    };
}
