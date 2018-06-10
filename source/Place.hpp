#pragma once

#include <fstream>

#include "lib/file_map.hpp"

namespace sjtu {
    class Places {
        FileMap<String, int> T;
        FileMap<int, String> T2;
        
    public:
        Places(): T("data_name_places"), T2("data_id_places") {}
        
        int Query(const String &name) {
            return T.query(name).first;
        }

        String QueryName(int idx) {
            return T2.query(idx).first;
        }
        
        void Insert(const String &name) {
            if (T.count(name)) return;
            T2.insert(T.Size() + 1, name);
            T.insert(name, T.Size() + 1);
        }

        int Size() const {
            return T.Size();
        }

        void Clear() {
            T.clear();
            T2.clear();
        }
    };
}
