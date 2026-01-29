#include <iostream>
#include <vector>
#include <string>

template <typename KeyType, typename ElemType, typename SumType>
struct GroupInfo {
    KeyType key{};
    std::size_t count{0};

    ElemType minElem{};
    ElemType maxElem{};

    SumType sum{};
    double avg{0.0};
};

template <typename K>
bool areKeysEqual(const K& a, const K& b) {
    return !(a < b) && !(b < a);
}

template <typename Elem, typename KeyFunc, typename ValueFunc>
auto groupStats(const Elem* arr, std::size_t n, KeyFunc keyFunc, ValueFunc valueFunc)
    -> std::vector<GroupInfo<
           decltype(keyFunc(*arr)),
           Elem,
           decltype(valueFunc(*arr))
       >>
{
    typedef decltype(keyFunc(*arr)) KeyType;
    typedef decltype(valueFunc(*arr)) SumType;
    typedef GroupInfo<KeyType, Elem, SumType> Group;
    std::vector<Group> groups;

    if (!arr || n == 0) {
        return groups;
    }

    for (std::size_t i = 0; i < n; ++i) {
        const Elem& x = arr[i];
        KeyType k = keyFunc(x);
        SumType v = valueFunc(x);

        bool found = false;
        for (std::size_t j = 0; j < groups.size(); ++j) {
            if (areKeysEqual(groups[j].key, k)) {
                Group& g = groups[j];

                g.count++;
                g.sum += v;

                if (x < g.minElem) g.minElem = x;
                if (g.maxElem < x) g.maxElem = x;

                found = true;
                break;
            }
        }

        if (!found) {
            Group newGroup;
            newGroup.key = k;
            newGroup.count = 1;
            newGroup.minElem = x;
            newGroup.maxElem = x;
            newGroup.sum = v;
            groups.push_back(newGroup);
        }
    }

    for (size_t i = 0; i < groups.size(); ++i) {
        if (groups[i].count > 0) {
            groups[i].avg = static_cast<double>(groups[i].sum) / groups[i].count;
        } else {
            groups[i].avg = 0.0;
        }
    }

    return groups;
}


template <typename GI>
void printGroups(const std::vector<GI>& groups,) {
    std::cout << "Key | Count | Min | Max | Sum | Avg\n";
    for (const auto& g : groups) {
        std::cout << g.key << " | "
                  << g.count << " | "
                  << g.minElem << " | "
                  << g.maxElem << " | "
                  << g.sum << " | "
                  << g.avg << "\n";
    }
    std::cout << "\n";
}

struct IntKey { 
    int operator()(int x) const { return x % 10; } 
};
struct IntVal { 
    int operator()(int x) const { return x; } 
};

struct DblKey { 
    int operator()(double x) const { return (x < 0) ? 0 : (x <= 10 ? 1 : 2); } 
};
struct DblVal { 
    double operator()(double x) const { return x; } 
};

struct StrKey { 
    char operator()(const std::string& s) const { return s.empty() ? '\0' : s[0]; } 
};
struct StrVal { 
    std::size_t operator()(const std::string& s) const { return s.size(); } 
};

int main() {
    int a1[] = { 12, 25, 17, 30, 41, 52, 63, 70, 99, 100 };
    std::cout << "--- Int Groups ---\n";
    printGroups(groupStats(a1, 10, IntKey(), IntVal()));

    double a2[] = { -1.5, 0.0, 3.3, 10.0, 12.2, -5.0, 7.7, 100.0 };
    std::cout << "\n--- Double Groups ---\n";
    printGroups(groupStats(a2, 8, DblKey(), DblVal()));

    std::string a3[] = { "who", "can", "sing", "that", "string", "check", "book" };
    std::cout << "\n--- String Groups ---\n";
    printGroups(groupStats(a3, 7, StrKey(), StrVal()));

    return 0;
}