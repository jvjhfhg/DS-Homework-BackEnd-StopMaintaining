#pragma once

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

        bool AddTrain(const String &tid, const String &name, char catalog, int stationCnt, int ticKindCnt, String *tickets, Station *stations) {
            return trains.Insert(tid, name, catalog, stationCnt, ticKindCnt, tickets, stations);
        }
        
        bool SaleTrain(const String &tid) {
            return trains.Sale(tid);
        }
        
        pair<Train, bool> QueryTrain(const String &tid) {
            return trains.Query(tid);
        }
        
        bool DeleteTrain(const String &tid) {
            return trains.Delete(tid);
        }
        
        bool ModifyTrain(const String &tid, const String &name, char catalog, int stationCnt, int ticKindCnt, String *tickets, Station *stations) {
            return trains.Modify(tid, name, catalog, stationCnt, ticKindCnt, tickets, stations);
        }
        
        /* 
         * System related
         */

        static void Clean() {
            users.Clear();
            trains.Clear();
        }
    };
}
