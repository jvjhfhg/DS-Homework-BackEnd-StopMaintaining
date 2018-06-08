#pragma once

#include "lib/algorithm.hpp"
#include "lib/utility.hpp"
#include "lib/vector.hpp"
#include "User.hpp"

namespace sjtu {
    Users users;

    class Interactor {
    public:
        /* 
         * User related
         */

        int Register(const char *name, const char *password, const char *email, const char *phone) {
            return users.Register(name, password, email, phone);
        }

        bool Login(int id, const char *password) {
            return users.Login(id, password);
        }

        // If second is false, the user doesn't exist. 
        pair<User, bool> QueryProfile(int id) {
            return users.Query(id);
        }

        bool ModifyProfile(int id, const char *name, const char *password, const char *email, const char *phone) {
            return users.Modify(id, name, password, email, phone);
        }

        bool Modify(int id1, int id2, User::Privilege priv) {
            return users.ModifyPrivilege(id1, id2, priv);
        }

        /* 
         * Train related
         */
    };
}
