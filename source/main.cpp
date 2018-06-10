// This is the main source file of backend test

#include <cstdio>
#include <iostream>
#include <cstdlib>

#include "railway_manager.hpp"
#include "lib/algorithm.hpp"
#include "lib/vector.hpp"

int main() {
    char s[45];
    sjtu::String opt;

    char a[45], b[45], c[45], d[45], e[45];
    int x, y, z;
    
#ifdef LOCAL
    int cnt = 0;
#endif

    while (~scanf("%s", s)) {
#ifdef LOCAL
        if (++cnt % 1000 == 0) std::cerr << cnt << std::endl;
#endif

        opt = s;

        // std::cerr << "cnbb: " << s << " -> " << opt.Str() << std::endl;

        if (opt == "register") {
            scanf("%s%s%s%s", a, b, c, d);
            // std::cerr << a << " " << b << " " << c << " " << d << std::endl;
            printf("%d\n", sjtu::Interactor::Register(a, b, c, d));
        } else if (opt == "login") {
            scanf("%d%s", &x, a);
            // std::cerr << x << " " << a << std::endl;
            printf("%d\n", (int)sjtu::Interactor::Login(x, a));
        } else if (opt == "query_profile") {
            scanf("%d", &x);
            auto res = sjtu::Interactor::QueryProfile(x);
            for (int i = 0; i < (int)res.size(); ++i)
                printf("%s%c", res[i].Str(), " \n"[i == (int)res.size() - 1]);
        } else if (opt == "modify_profile") {
            scanf("%d%s%s%s%s", &x, a, b, c, d);
            printf("%d\n", (int)sjtu::Interactor::ModifyProfile(x, a, b, c, d));
        } else if (opt == "modify_privilege") {
            scanf("%d%d%d", &x, &y, &z);
            if (z != 1 && z != 2) puts("0");
            else printf("%d\n", (int)sjtu::Interactor::ModifyPrivilege(x, y, z));
        } else if (opt == "query_ticket") {
            scanf("%s%s%s%s", a, b, c, d);
            auto res = sjtu::Interactor::QueryTicket(a, b, c, d);
            if (res.empty()) puts("0");
            else {
                printf("%d\n", (int)res.size());
                for (int i = 0; i < (int)res.size(); ++i)
                    for (int j = 0; j < (int)res[i].size(); ++j)
                        printf("%s%c", res[i][j].Str(), " \n"[j == (int)res[i].size() - 1]);
            }
        } else if (opt == "query_transfer") {
            scanf("%s%s%s%s", a, b, c, d);

            auto res = sjtu::Interactor::QueryTransfer(a, b, c, d);
            if (res.first.size() == 0) {
                puts("-1");
            } else {
                for (int i = 0; i < (int)res.first.size(); ++i)
                    printf("%s%c", res.first[i].Str(), " \n"[i == (int)res.first.size() - 1]);
                for (int i = 0; i < (int)res.second.size(); ++i)
                    printf("%s%c", res.second[i].Str(), " \n"[i == (int)res.second.size() - 1]);
            }

            // puts("啤酒真凉，我还坐在大钟旁，真吊。");
        } else if (opt == "buy_ticket") {
            scanf("%d%d%s%s%s%s%s", &x, &y, a, b, c, d, e);
            printf("%d\n", (int)sjtu::Interactor::BuyTicket(x, y, a, b, c, d, e));
        } else if (opt == "query_order") {
            scanf("%d%s%s", &x, a, b);
            auto res = sjtu::Interactor::QueryOrder(x, a, b);
            if (res.empty()) puts("0");
            else {
                printf("%d\n", (int)res.size());
                for (int i = 0; i < (int)res.size(); ++i)
                    for (int j = 0; j < (int)res[i].size(); ++j)
                        printf("%s%c", res[i][j].Str(), " \n"[j == (int)res[i].size() - 1]);
            }
        } else if (opt == "refund_ticket") {
            scanf("%d%d%s%s%s%s%s", &x, &y, a, b, c, d, e);
            printf("%d\n", (int)sjtu::Interactor::RefundTicket(x, y, a, b, c, d, e));
        } else if (opt == "add_train") {
            sjtu::String tid, name, catalog, stationCnt, priceCnt, tmp;
            sjtu::vector<sjtu::String> commands;
            tid.Read();
            name.Read();
            catalog.Read();
            stationCnt.Read();
            priceCnt.Read();
            commands.push_back(tid);
            commands.push_back(name);
            commands.push_back(catalog);
            commands.push_back(stationCnt);
            commands.push_back(priceCnt);
            for (int i = priceCnt.ToInt(); i--; ) {
                tmp.Read();
                commands.push_back(tmp);
            }
            for (int i = stationCnt.ToInt(); i--; ) {
                for (int j = 4 + priceCnt.ToInt(); j--; ) {
                    tmp.Read();
                    commands.push_back(tmp);
                }
            }
            printf("%d\n", (int)sjtu::Interactor::AddTrain(commands));
        } else if (opt == "sale_train") {
            scanf("%s", a);
            printf("%d\n", (int)sjtu::Interactor::SaleTrain(a));
        } else if (opt == "query_train") {
            scanf("%s", a);
            auto res = sjtu::Interactor::QueryTrain(a);
            for (int i = 0; i < (int)res.size(); ++i)
                printf("%s%c", res[i].Str(), " \n"[i == (int)res.size() - 1]);
        } else if (opt == "delete_train") {
            scanf("%s", a);
            printf("%d\n", (int)sjtu::Interactor::DeleteTrain(a));
        } else if (opt == "modify_train") {
            sjtu::String tid, name, catalog, stationCnt, priceCnt, tmp;
            sjtu::vector<sjtu::String> commands;
            tid.Read();
            name.Read();
            catalog.Read();
            stationCnt.Read();
            priceCnt.Read();
            commands.push_back(tid);
            commands.push_back(name);
            commands.push_back(catalog);
            commands.push_back(stationCnt);
            commands.push_back(priceCnt);
            for (int i = priceCnt.ToInt(); i--; ) {
                tmp.Read();
                commands.push_back(tmp);
            }
            for (int i = stationCnt.ToInt(); i--; ) {
                for (int j = 4 + priceCnt.ToInt(); j--; ) {
                    tmp.Read();
                    commands.push_back(tmp);
                }
            }
            printf("%d\n", (int)sjtu::Interactor::ModifyTrain(commands));
        } else if (opt == "clean") {
            sjtu::Interactor::Clean();
            puts("1");
        } else if (opt == "exit") {
            puts("BYE");
#ifndef LOCAL
            exit(0);
#endif
        }
    }

    return 0;
}
