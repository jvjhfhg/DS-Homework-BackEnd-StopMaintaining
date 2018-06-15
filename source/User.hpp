#pragma once

#include <cstdio>
#include <iostream>
#include <fstream>

#include "lib/algorithm.hpp"
#include "lib/b_plus_tree.hpp"
#include "lib/utility.hpp"

namespace sjtu
{
    class User
    {
    public:
        String name;
        String password;
        String email;
        String phone;
        int priv;
        int id;

        User() = default;

        User(const char *_name, const char *_password, const char *_email, const char *_phone, int _id):
            name(_name), password(_password), email(_email), phone(_phone), id(_id)
        {
            if (id == 2018) priv = 2;
            else priv = 1;
        }

        void Modify(const char *_name, const char *_password, const char *_email, const char *_phone)
        {
            name = _name;
            password = _password;
            email = _email;
            phone = _phone;
        }
    };

    class Users
    {
    private:
        BPTree<int, User> T;
        int curid;

    public:
        Users(): T("data_users.rwdb")
        {
            std::fstream file("data_users_id.rwdb", std::fstream::in);
            if (!file)
                curid = 2018;
            else
                file >> curid;
            file.close();
        }

        ~Users()
        {
            std::fstream idFile("data_users_id.rwdb", std::fstream::out);
            idFile << curid << std::endl;
            idFile.close();
        }

        int Register(const char *name, const char *password, const char *email, const char *phone)
        {
            T.insert(curid, User(name, password, email, phone, curid));
            return curid++;
        }

        bool Login(int id, const String &password)
        {
            // std::cerr << "xixi" << std::endl;
            auto user = T.query(id);
            // std::cerr << "cnbb" << std::endl;
            if (user.second == false) return false;
            return user.first.password == password;
        }

        pair<User, bool> Query(int id)
        {
            return T.query(id);
        }

        bool Modify(int id, const char *name, const char *password, const char *email, const char *phone)
        {
            auto user = T.query(id);
            if (user.second == false) return false;
            user.first.Modify(name, password, email, phone);
            T.modify(id, user.first);
            return true;
        }

        bool ModifyPrivilege(int id1, int id2, int priv)
        {
            auto user = T.query(id1);
            if (user.second == false || user.first.priv != 2) return false;
            user = T.query(id2);
            if (user.second == false || (user.first.priv == 2 && priv == 1)) return false;
            user.first.priv = priv;
            T.modify(id2, user.first);
            return true;
        }

        void Clear()
        {
            T.clear();
            curid = 2018;
        }
    };
}
