// This is the main source file of backend test

#include <cstdio>
#include <iostream>

#include "railway_manager.hpp"
#include "lib/algorithm.hpp"
#include "lib/vector.hpp"

int main() {
    char s[45];
    sjtu::String opt;

    char a[45], b[45], c[45], d[45];
    int x, y, z;
    
    // int cnt = 0;

    while (~scanf("%s", s)) {
        // std::cerr << ++cnt << std::endl;

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
            if (res.second == false) puts("0");
            else printf("%s %s %s %d\n", res.first.name.Str(), res.first.email.Str(), res.first.phone.Str(), (int)res.first.priv);
        } else if (opt == "modify_profile") {
            scanf("%d%s%s%s%s", &x, a, b, c, d);
            printf("%d\n", (int)sjtu::Interactor::ModifyProfile(x, a, b, c, d));
        } else if (opt == "modify_privilege") {
            scanf("%d%d%d", &x, &y, &z);
            if (z != 1 && z != 2) puts("0");
            else printf("%d\n", (int)sjtu::Interactor::ModifyPrivilege(x, y, z));
        }
        // TODO
        else if (opt == "clean") {
            sjtu::Interactor::Clean();
            puts("1");
        } else if (opt == "exit") {
            puts("BYE");
            // exit(0);
        }
    }

    return 0;
}
