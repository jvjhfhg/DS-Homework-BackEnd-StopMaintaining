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

        static int Register(const char *name, const char *password, const char *email, const char *phone) {
            return users.Register(name, password, email, phone);
        }

        static bool Login(int id, const char *password) {
            return users.Login(id, password);
        }

        // If second is false, the user doesn't exist. 
        static pair<User, bool> QueryProfile(int id) {
            return users.Query(id);
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


        
        /* 
         * Train related
         */


        
        /* 
         * System related
         */

        static void Clean() {
            users.Clear();
        }
    };
}
