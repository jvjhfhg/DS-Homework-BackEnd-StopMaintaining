#pragma once

#include <fstream>

#include "lib/file_map.hpp"

namespace sjtu
{
    class Places
    {
        FileMap<String, int> T1;
        FileMap<int, String> T2;

    public:
        Places(): T1("data_name_places.rwdb"), T2("data_id_places.rwdb") {}

        int Query(const String &name)
        {
            return T1.query(name).first;
        }

        String QueryName(int idx)
        {
            return T2.query(idx).first;
        }

        void Insert(const String &name)
        {
            if (T1.count(name)) return;
            T2.insert(T1.size() + 1, name);
            T1.insert(name, T1.size() + 1);
        }

        int Size() const
        {
            return T1.size();
        }

        void Clear()
        {
            T1.clear();
            T2.clear();
        }
    };
}
