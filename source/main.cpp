// This is the main source file of backend test

#include <cstdio>

#include "railway_manager.hpp"
#include "lib/algorithm.hpp"

int main() {
    char s[105];
    sjtu::string opt;

    char a[105], b[105], c[105], d[105];
    int x, y, z;

    sjtu::Interactor::Clean();

    while (~scanf("%s", s)) {
        opt = s;
        if (opt == "register") {
            scanf("%s%s%s%s", a, b, c, d);
            printf("%d\n", sjtu::Interactor::Register(a, b, c, d));
        } else if (opt == "login") {
            scanf("%d%s", &x, a);
            printf("%d\n", sjtu::Interactor::Login(x, a));
        } else if (opt == "query_profile") {
            scanf("%d", &x);
            auto res = sjtu::Interactor::QueryProfile(x);
            if (res.second == false) puts("0");
            else printf("%s %s %s %d\n", res.first._User_Name, res.first._Email, res.first._Phone, res.first._Privilege);
        } else if (opt == "modify_profile") {
            scanf("%d%s%s%s%s", &x, a, b, c, d);
            printf("%d\n", sjtu::Interactor::ModifyProfile(x, a, b, c, d));
        } else if (opt == "modify_privilege") {
            scanf("%d%d%d", &x, &y, &z);
            if (z < 1 || z > 2) puts("0");
            else printf("%d\n", sjtu::Interactor::ModifyPrivilege(x, y, z));
        }
    }

    return 0;
}