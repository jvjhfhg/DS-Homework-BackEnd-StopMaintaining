#define _CRT_SECURE_NO_WARNINGS
#pragma once

#include <algorithm>

#include "lib/algorithm.hpp"
#include "lib/utility.hpp"
#include "lib/vector.hpp"

#include "user.hpp"
#include "train.hpp"
#include "ticket.hpp"
#include "place.hpp"

namespace sjtu
{
    Places places;
    Users users;
    Trains trains;
    OrderTime orderTime;
    OrderUser orderUser;

    class Interactor
    {
    public:
        /*
         * User related
         */

        static int Register(const char *name, const char *password, const char *email, const char *phone)
        {
            return users.Register(name, password, email, phone);
        }

        static bool Login(int id, const char *password)
        {
            return users.Login(id, password);
        }

        static vector<String> QueryProfile(int id)
        {
            vector<String> res;
            auto t = users.Query(id);
            if (t.second == false)
                res.push_back("0");
            else
            {
                res.push_back(t.first.name);
                res.push_back(t.first.email);
                res.push_back(t.first.phone);
                res.push_back(String::Int(t.first.priv));
            }
            return res;
        }

        static bool ModifyProfile(int id, const char *name, const char *password, const char *email, const char *phone)
        {
            return users.Modify(id, name, password, email, phone);
        }

        static bool ModifyPrivilege(int id1, int id2, int priv)
        {
            return users.ModifyPrivilege(id1, id2, priv);
        }

        /*
         * Ticket related
         */

        static vector<vector<String>> _QueryTicket(const String &loc1, const String &loc2, const Date &date, char catalog)
        {
            int l1 = places.Query(loc1), l2 = places.Query(loc2);
            auto t = trains.tickets.Query(l1, l2, catalog);
            vector<vector<String>> res;
            if (t.second == false || l1 == 0 || l2 == 0)
            {
                return res;
            }
            for (int i = 0; i < (int)t.first.size(); ++i)
            {
                String tid = t.first[i];
                Train train = trains.Query(tid).first;
                res.push_back(vector<String>());
                vector<String> &vec = res[res.size() - 1];
                vec.push_back(tid);

                for (int j = 0; j < train.stationCnt; ++j) if (train.stations[j].name == l1)
                    {
                        vec.push_back(loc1);
                        vec.push_back(date.ToString());
                        vec.push_back(train.stations[j].startTime.ToString());

                        int deltaDay = 0;
                        for (int k = j + 1; k < train.stationCnt; ++k)
                        {
                            if (train.stations[k].name == l2)
                            {
                                if (train.stations[k].arriveTime < train.stations[k - 1].startTime) ++deltaDay;

                                vec.push_back(loc2);
                                // vec.push_back((date + deltaDay).ToString());
                                vec.push_back(date.ToString());
                                vec.push_back(train.stations[k].arriveTime.ToString());

                                for (int tk = 0; tk < train.ticketKindCnt; ++tk)
                                {
                                    vec.push_back(train.tickets[tk]);

                                    int ticketLeft = 2000;
                                    double price = 0;
                                    deltaDay = 0;
                                    for (int sta = j + 1; sta <= k; ++sta)
                                    {
                                        if ((sta != k && train.stations[sta].startTime < train.stations[sta - 1].startTime) ||
                                                (sta == k && train.stations[sta].arriveTime < train.stations[sta - 1].startTime)) ++deltaDay;
                                        // ticketLeft = std::min(ticketLeft, 2000 - orderTime.Query(tid, tk, sta, date + deltaDay));
                                        ticketLeft = std::min(ticketLeft, 2000 - orderTime.Query(tid, tk, sta, date));
                                        price += train.stations[sta].price[tk];
                                    }

                                    vec.push_back(String::Int(ticketLeft));
                                    static char tmp[45];
                                    // sprintf(tmp, "%s%f", train.stations[k].currency[tk].Str(), price);
                                    sprintf(tmp, "%f", price);
                                    // vec.push_back(train.stations[sta].currency[tk]);
                                    // vec.push_back(String::Float(price));
                                    vec.push_back(tmp);
                                }

                                break;
                            }
                            else if (train.stations[k].startTime < train.stations[k - 1].startTime) ++deltaDay;
                        }

                        break;
                    }
            }
            return res;
        }

        static vector<vector<String>> QueryTicket(const String &loc1, const String &loc2, const Date &date, const char *catalogs)
        {
            vector<vector<String>> res;
            for (int i = 0, len = strlen(catalogs); i < len; ++i)
            {
                char catalog = catalogs[i];
                auto tmp = _QueryTicket(loc1, loc2, date, catalog);
                for (int i = 0; i < (int)tmp.size(); ++i)
                    res.push_back(tmp[i]);
            }
            return res;
        }

        static pair<int, Time> Calc(const Time &t1, const Time &t2, const Time &t3, const Time &t4)
        {
            pair<int, Time> res;

            if (t2 < t1) ++res.first;
            res.second = res.second + (t2 - t1);
            if (res.second < t2 - t1) ++res.first;

            if (t3 < t2) ++res.first;
            res.second = res.second + (t3 - t2);
            if (res.second < t3 - t2) ++res.first;

            if (t4 < t3) ++res.first;
            res.second = res.second + (t4 - t3);
            if (res.second < t4 - t3) ++res.first;

            return res;
        }

        static pair<vector<String>, vector<String>> QueryTransfer(const String &loc1, const String &loc2, const Date &date, const char *catalogs)
        {
            pair<vector<String>, vector<String>> res;
            pair<int, Time> duringTime(10000, "23:59");
            int l1 = places.Query(loc1), l2 = places.Query(loc2);
            int placeCnt = places.Size();
            for (int i = 0, len = strlen(catalogs); i < len; ++i)
            {
                char catalog = catalogs[i];
                for (int mid = 1; mid <= placeCnt; ++mid) if (mid != l1 && mid != l2)
                    {
                        String midName = places.QueryName(mid);
                        auto t1 = trains.tickets.Query(l1, mid, catalog);
                        auto t2 = trains.tickets.Query(mid, l2, catalog);
                        if (t1.second == false || t2.second == false) continue;

                        for (int i1 = 0; i1 < (int)t1.first.size(); ++i1)
                            for (int i2 = 0; i2 < (int)t2.first.size(); ++i2)
                            {
                                Train tr1 = trains.Query(t1.first[i1]).first;
                                Train tr2 = trains.Query(t2.first[i2]).first;

                                int S1, T1, S2, T2;
                                for (int i = 0; i < tr1.stationCnt; ++i)
                                {
                                    if (tr1.stations[i].name == l1) S1 = i;
                                    if (tr1.stations[i].name == mid) T1 = i;
                                }
                                for (int i = 0; i < tr2.stationCnt; ++i)
                                {
                                    if (tr2.stations[i].name == mid) S2 = i;
                                    if (tr2.stations[i].name == l2) T2 = i;
                                }

                                int deltaDay1 = 0, deltaDay2 = 0;
                                for (int sta = S1 + 1; sta <= T1; ++sta)
                                {
                                    if ((sta != T1 && tr1.stations[sta].startTime < tr1.stations[sta - 1].startTime) ||
                                            (sta == T1 && tr1.stations[sta].arriveTime < tr1.stations[sta - 1].startTime)) ++deltaDay1;
                                }
                                for (int sta = S2 + 1; sta <= T2; ++sta)
                                {
                                    if ((sta != T2 && tr2.stations[sta].startTime < tr2.stations[sta - 1].startTime) ||
                                            (sta == T2 && tr2.stations[sta].arriveTime < tr2.stations[sta - 1].startTime)) ++deltaDay2;
                                }

                                auto tt = Calc(tr1.stations[S1].startTime, tr1.stations[T1].arriveTime, tr2.stations[S2].startTime, tr2.stations[T2].arriveTime);
                                int deltaDay = tt.first;
                                tt.first += deltaDay1 + deltaDay2;

                                if (tt < duringTime)
                                {
                                    duringTime = tt;
                                    res.first.clear();
                                    res.second.clear();

                                    res.first.push_back(tr1.id);
                                    res.first.push_back(loc1);
                                    res.first.push_back(date.ToString());
                                    res.first.push_back(tr1.stations[S1].startTime.ToString());
                                    res.first.push_back(midName);
                                    // res.first.push_back((date + deltaDay1).ToString());
                                    res.first.push_back(date.ToString());
                                    res.first.push_back(tr1.stations[T1].arriveTime.ToString());
                                    for (int tk = 0; tk < tr1.ticketKindCnt; ++tk)
                                    {
                                        int ticCnt = 2000, tdel = 0;
                                        double price = 0;
                                        for (int sta = S1 + 1; sta <= T1; ++sta)
                                        {
                                            if ((sta != T1 && tr1.stations[sta].startTime < tr1.stations[sta - 1].startTime) ||
                                                    (sta == T1 && tr1.stations[sta].arriveTime < tr1.stations[sta - 1].startTime)) ++tdel;
                                            // ticCnt = std::min(ticCnt, 2000 - orderTime.Query(tr1.id, tk, sta, (date + tdel).ToString()));
                                            ticCnt = std::min(ticCnt, 2000 - orderTime.Query(tr1.id, tk, sta, date.ToString()));
                                            price += tr1.stations[sta].price[tk];
                                        }
                                        res.first.push_back(tr1.tickets[tk]);
                                        res.first.push_back(String::Int(ticCnt));
                                        static char tmp[45];
                                        // sprintf(tmp, "%s%f", tr1.stations[T1].currency[tk].Str(), price);
                                        sprintf(tmp, "%f", price);
                                        res.first.push_back(tmp);
                                    }

                                    res.second.push_back(tr2.id);
                                    res.second.push_back(midName);
                                    // res.second.push_back((date + deltaDay1 + deltaDay).ToString());
                                    res.second.push_back(date.ToString());
                                    res.second.push_back(tr2.stations[S2].startTime.ToString());
                                    res.second.push_back(loc2);
                                    // res.second.push_back((date + deltaDay1 + deltaDay + deltaDay2).ToString());
                                    res.second.push_back(date.ToString());
                                    res.second.push_back(tr2.stations[T2].arriveTime.ToString());
                                    for (int tk = 0; tk < tr2.ticketKindCnt; ++tk)
                                    {
                                        int ticCnt = 2000, tdel = 0;
                                        double price = 0;
                                        for (int sta = S2 + 1; sta <= T2; ++sta)
                                        {
                                            if ((sta != T2 && tr2.stations[sta].startTime < tr2.stations[sta - 1].startTime) ||
                                                    (sta == T2 && tr2.stations[sta].arriveTime < tr2.stations[sta - 1].startTime)) ++tdel;
                                            // ticCnt = std::min(ticCnt, 2000 - orderTime.Query(tr2.id, tk, sta, (date + deltaDay1 + deltaDay + tdel).ToString()));
                                            ticCnt = std::min(ticCnt, 2000 - orderTime.Query(tr2.id, tk, sta, date.ToString()));
                                            price += tr2.stations[sta].price[tk];
                                        }
                                        res.second.push_back(tr2.tickets[tk]);
                                        res.second.push_back(String::Int(ticCnt));
                                        static char tmp[45];
                                        // sprintf(tmp, "%s%f", tr2.stations[T2].currency[tk].Str(), price);
                                        sprintf(tmp, "%f", price);
                                        res.second.push_back(tmp);
                                    }
                                }
                            }
                    }
            }
            return res;
        }

        static bool BuyTicket(int id, int num, const String &tid, const String &loc1, const String &loc2, const Date &date, const String &ticketKind)
        {
            int l1 = places.Query(loc1), l2 = places.Query(loc2);
            auto t = trains.Query(tid);
            if (t.second == false || l1 == 0 || l2 == 0) return false;
            Train train = t.first;
            int tk = -1;
            for (int i = 0; i < train.ticketKindCnt; ++i)
                if (ticketKind == train.tickets[i])
                {
                    tk = i;
                    break;
                }
            if (tk == -1) return false;
            for (int i = 0; i < train.stationCnt; ++i) if (train.stations[i].name == l1)
                {
                    for (int j = i + 1; j < train.stationCnt; ++j) if (train.stations[j].name == l2)
                        {
                            int ticketLeft = 2000, deltaDay = 0;
                            for (int sta = i + 1; sta <= j; ++sta)
                            {
                                if ((sta != j && train.stations[sta].startTime < train.stations[sta - 1].startTime) ||
                                        (sta == j && train.stations[sta].arriveTime < train.stations[sta - 1].startTime)) ++deltaDay;
                                // ticketLeft = std::min(ticketLeft, 2000 - orderTime.Query(tid, tk, sta, date + deltaDay));
                                ticketLeft = std::min(ticketLeft, 2000 - orderTime.Query(tid, tk, sta, date));
                            }
                            if (ticketLeft < num) return false;
                            deltaDay = 0;
                            for (int sta = i + 1; sta <= j; ++sta)
                            {
                                if ((sta != j && train.stations[sta].startTime < train.stations[sta - 1].startTime) ||
                                        (sta == j && train.stations[sta].arriveTime < train.stations[sta - 1].startTime)) ++deltaDay;
                                // orderTime.Add(tid, tk, sta, date + deltaDay, num);
                                orderTime.Add(tid, tk, sta, date, num);
                            }

                            int tmp[] = {0, 0, 0, 0, 0};
                            tmp[tk] = num;

                            // orderUser.Add(id, tid, train.catalog, l1, date, train.stations[i].startTime, l2, date + deltaDay, train.stations[j].arriveTime, tmp);
                            orderUser.Add(id, tid, train.catalog, l1, date, train.stations[i].startTime, l2, date, train.stations[j].arriveTime, tmp);

                            return true;
                        }
                    break;
                }
            return false;
        }

        static vector<vector<String>> _QueryOrder(int id, const Date &date, char catalog)
        {
            auto t = orderUser.Query(id, date, catalog);
            vector<vector<String>> res;
            for (int i = 0; i < (int)t.size(); ++i)
            {
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
                for (int j = 0; j < train.ticketKindCnt; ++j)
                {
                    vec.push_back(train.tickets[j]);
                    vec.push_back(String::Int(t[i].second.cnt[j]));
                    for (int st1 = 0; st1 < train.stationCnt; ++st1) if (train.stations[st1].name == t[i].second.loc1)
                        {
                            for (int st2 = st1 + 1; st2 < train.stationCnt; ++st2) if (train.stations[st2].name == t[i].second.loc2)
                                {
                                    double price = 0;
                                    for (int sta = st1 + 1; sta <= st2; ++sta)
                                        price += train.stations[sta].price[j];
                                    static char tmp[45];
                                    // sprintf(tmp, "%s%f", train.stations[st2].currency[j].Str(), price);
                                    sprintf(tmp, "%f", price);
                                    // vec.push_back(train.stations[sta].currency[tk]);
                                    // vec.push_back(String::Float(price));
                                    vec.push_back(tmp);
                                    break;
                                }
                            break;
                        }
                }
            }
            return res;
        }

        static vector<vector<String>> QueryOrder(int id, const Date &date, const char *catalogs)
        {
            vector<vector<String>> res;
            for (int i = 0, len = strlen(catalogs); i < len; ++i)
            {
                char catalog = catalogs[i];
                auto tmp = _QueryOrder(id, date, catalog);
                for (int i = 0; i < (int)tmp.size(); ++i)
                    res.push_back(tmp[i]);
            }
            return res;
        }

        static bool RefundTicket(int id, int num, const String &tid, const String &loc1, const String &loc2, const Date &date, const String &ticketKind)
        {
            int l1 = places.Query(loc1), l2 = places.Query(loc2);
            auto t = trains.Query(tid);
            if (t.second == false || l1 == 0 || l2 == 0) return false;
            Train train = t.first;
            int tk = -1;
            for (int i = 0; i < train.ticketKindCnt; ++i)
                if (ticketKind == train.tickets[i])
                {
                    tk = i;
                    break;
                }
            if (tk == -1) return false;
            for (int i = 0; i < train.stationCnt; ++i) if (train.stations[i].name == l1)
                {
                    for (int j = i + 1; j < train.stationCnt; ++j) if (train.stations[j].name == l2)
                        {
                            const int *ticketBought = orderUser.QueryActualTicket(id, tid, date, train.catalog, l1, l2);
                            if (ticketBought[tk] < num) return false;
                            int deltaDay = 0;
                            for (int sta = i + 1; sta <= j; ++sta)
                            {
                                if ((sta != j && train.stations[sta].startTime < train.stations[sta - 1].startTime) ||
                                        (sta == j && train.stations[sta].arriveTime < train.stations[sta - 1].startTime)) ++deltaDay;
                                // orderTime.Add(tid, tk, sta, date + deltaDay, -num);
                                orderTime.Add(tid, tk, sta, date, -num);
                            }

                            int tmp[] = {0, 0, 0, 0, 0};
                            tmp[tk] = -num;

                            // orderUser.Add(id, tid, train.catalog, l1, date, train.stations[i].startTime, l2, date + deltaDay, train.stations[j].arriveTime, tmp);
                            orderUser.Add(id, tid, train.catalog, l1, date, train.stations[i].startTime, l2, date, train.stations[j].arriveTime, tmp);

                            return true;
                        }
                    break;
                }
            return false;
        }

        /*
         * Train related
         */

        static void Read(const char *s, char *a, double &b)
        {
            for (int i = 0; s[i]; ++i)
            {
                if (s[i] >= '0' && s[i] <= '9')
                {
                    sscanf(s + i, "%lf", &b);
                    a[i] = '\0';
                    return;
                }
                a[i] = s[i];
            }
        }

        static bool AddTrain(const vector<String> &vs)
        {
            int id = 0;
            String tid = vs[id++];
            String name = vs[id++];
            char catalog = *vs[id++].Str();
            int stationCnt = vs[id++].ToInt();
            int ticKindCnt = vs[id++].ToInt();
            String tickets[5];
            for (int i = 0; i < ticKindCnt; ++i)
                tickets[i] = vs[id++];
            Station stations[60];
            for (int i = 0; i < stationCnt; ++i)
            {
                if (places.Query(vs[id]) == 0)
                    places.Insert(vs[id]);
                stations[i].name = places.Query(vs[id++]);
                stations[i].arriveTime = (i == 0 ? Time("00:00") : Time(vs[id].Str()));
                ++id;
                stations[i].startTime = (i == stationCnt - 1 ? Time("00:00") : Time(vs[id].Str()));
                ++id;
                ++id;
                for (int j = 0; j < ticKindCnt; ++j)
                {
                    static char tmp[45];
                    Read(vs[id++].Str(), tmp, stations[i].price[j]);
                    stations[i].currency[j] = tmp;
                }
            }
            return trains.Insert(tid, name, catalog, stationCnt, ticKindCnt, tickets, stations);
        }

        static bool SaleTrain(const String &tid)
        {
            return trains.Sale(tid);
        }

        static vector<vector<String>> QueryTrain(const String &tid)
        {
            vector<vector<String>> res;
            res.push_back(vector<String>());
            auto t = trains.Query(tid);
            if (t.second == false || t.first.status == 0)
                res[0].push_back("0");
            else
            {
                res[0].push_back(t.first.id);
                res[0].push_back(t.first.name);
                char tmp[45];
                tmp[0] = t.first.catalog;
                tmp[1] = '\0';
                res[0].push_back(tmp);
                res[0].push_back(String::Int(t.first.stationCnt));
                res[0].push_back(String::Int(t.first.ticketKindCnt));
                for (int i = 0; i < t.first.ticketKindCnt; ++i)
                    res[0].push_back(t.first.tickets[i]);
                for (int i = 0; i < t.first.stationCnt; ++i)
                {
                    res.push_back(vector<String>());
                    vector<String> &vec = res[res.size() - 1];
                    vec.push_back(places.QueryName(t.first.stations[i].name));
                    if (i == 0) vec.push_back("xx:xx");
                    else vec.push_back(t.first.stations[i].arriveTime.ToString());
                    if (i == t.first.stationCnt - 1) vec.push_back(t.first.stations[i].arriveTime.ToString());
                    else vec.push_back(t.first.stations[i].startTime.ToString());
                    if (i == 0 || i == t.first.stationCnt - 1) vec.push_back("xx:xx");
                    else vec.push_back((t.first.stations[i].startTime - t.first.stations[i].arriveTime).ToString());
                    for (int j = 0; j < t.first.ticketKindCnt; ++j)
                    {
                        static char tmp[45];
                        sprintf(tmp, "%s%f", t.first.stations[i].currency[j].Str(), t.first.stations[i].price[j]);
                        vec.push_back(tmp);
                    }
                }
            }
            return res;
        }

        static bool DeleteTrain(const String &tid)
        {
            return trains.Delete(tid);
        }

        static bool ModifyTrain(const vector<String> &vs)
        {
            int id = 0;
            String tid = vs[id++];
            String name = vs[id++];
            char catalog = *vs[id++].Str();
            int stationCnt = vs[id++].ToInt();
            int ticKindCnt = vs[id++].ToInt();
            String tickets[5];
            for (int i = 0; i < ticKindCnt; ++i)
                tickets[i] = vs[id++];
            Station stations[60];
            for (int i = 0; i < stationCnt; ++i)
            {
                if (places.Query(vs[id]) == 0)
                    places.Insert(vs[id]);
                stations[i].name = places.Query(vs[id++]);
                stations[i].arriveTime = (i == 0 ? Time("00:00") : Time(vs[id].Str()));
                ++id;
                stations[i].startTime = (i == stationCnt - 1 ? Time("00:00") : Time(vs[id].Str()));
                ++id;
                ++id;
                for (int j = 0; j < ticKindCnt; ++j)
                {
                    static char tmp[45];
                    Read(vs[id++].Str(), tmp, stations[i].price[j]);
                    stations[i].currency[j] = tmp;
                }
            }
            return trains.Modify(tid, name, catalog, stationCnt, ticKindCnt, tickets, stations);
        }

        /*
         * System related
         */

        static void Clean()
        {
            places.Clear();
            users.Clear();
            trains.Clear();
            orderTime.Clear();
            orderUser.Clear();
        }
    };
}
