#pragma once

#include <cstdio>
#include <iostream>
#include <fstream>

#include "lib/algorithm.hpp"
#include "lib/b_plus_tree.hpp"
#include "lib/utility.hpp"

namespace sjtu {
    class User {
    public:
        enum Privilege {
            Unregistered = 0, Normal = 1, Admin = 2
        };

        String name;
        String password;
        String email;
        String phone;
        Privilege priv;
        int id;

        User() = default;

        User(const char *_name, const char *_password, const char *_email, const char *_phone, int _id):
            name(_name), password(_password), email(_email), phone(_phone), id(_id) {
            if (id == 2018) priv = Privilege::Admin;
            else priv = Privilege::Normal;
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
            std::fstream file("data_users_id", std::fstream::in);
            if (!file)
                currentID = 2018;
            else
                file >> currentID;
            file.close();
        }

        ~Users() {
            std::fstream idFile("data_users_id", std::fstream::out);
            idFile << currentID << std::endl;
            // std::cerr << currentID << std::endl;
            idFile.close();
        }
        
        int Register(const char *name, const char *password, const char *email, const char *phone) {
            // std::cerr << "in" << std::endl;
            T.insert(currentID, User(name, password, email, phone, currentID));
            // std::cerr << "out" << std::endl;
            return currentID++;
        }

        bool Login(int id, const String &password) {
            // std::cerr << "xixi" << std::endl;
            auto user = T.query(id);
            // std::cerr << "cnbb" << std::endl;
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
            auto user = T.query(id1);
            if (user.second == false || user.first.priv != User::Privilege::Admin) return false;
            user = T.query(id2);
            if (user.second == false || (user.first.priv == User::Privilege::Admin && priv == User::Privilege::Normal)) return false;
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
