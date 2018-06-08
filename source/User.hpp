#pragma once

#include <cstdio>

#include "lib/algorithm.hpp"
#include "lib/b_plus_tree.hpp"
#include "lib/utility.hpp"

namespace sjtu {
    class User {
        enum Privilege {
            Unregistered = 0, User = 1, Admin = 2
        };

    public:
        String name;
        String password;
        String email;
        String phone;
        Privilege priv;
        int id;

        User() = default;
        User(const char *name, const char *password, const char *email, const char *phone, int id):
            name(name), password(password), email(email), phone(phone), id(id) {
            if (id == 2018) priv = Privilege::Admin;
            else priv = Privilege::User;
        }

        void Modify(const char *_name, const char *_password, const char *_email, const char *_phone) {
            name = _name;
            password = _password;
            email = _email;
            phone = _phone;
        }
    };

    class Users {
    private:
        BPTree<int, User> T;
        int currentID;
        
    public:
        Users(): T("data_users") {
            fopen("data_users", "w");
            currentID = 2018;
        }
        
        int Register(const char *name, const char *password, const char *email, const char *phone) {
            T.insert(currentID, User(name, password, email, phone, currentID));
            return currentID++;
        }

        bool Login(int id, const String &password) {
            auto user = T.query(id);
            if (user.second == false) return false;
            return user.first.password == password;
        }

        pair<User, bool> Query(int id) {
            return T.query(id);
        }

        bool Modify(int id, const char *name, const char *password, const char *email, const char *phone) {
            auto user = T.query(id);
            if (user.second == false) return false;
            user.first.Modify(name, password, email, phone);
            T.modify(id, user.first);
            return true;
        }

        bool ModifyPrivilege(int id1, int id2, User::Privilege priv) {
            if (priv == User::Privilege::User) return false;
            auto user = T.query(id1);
            if (user.second == false || user.first.priv != User::Privilege::Admin) return false;
            auto user = T.query(id2);
            if (user.second == false || user.first.priv == User::Privilege::Admin) return false;
            user.first.priv = priv;
            T.modify(id2, user.first);
            return true;
        }

        void Clear() {
            T.clear();
            currentID = 2018;
        }
    };
}
