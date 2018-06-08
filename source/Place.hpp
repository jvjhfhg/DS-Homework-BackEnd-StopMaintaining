#pragma once

#include <fstream>

#include "lib/fake_b_plus_tree.hpp"

namespace sjtu {
    class Places {
        BPTree<String, int> T;
        
    public:
        Places(): T("data/name_places") {}
        
        int Query(const String &name) {
            return T.query(name).first;
        }
        
        void Insert(const String &name) {
            if (T.count(name)) return;
            T.insert(name, T.Size() + 1);
        }
    };
}
