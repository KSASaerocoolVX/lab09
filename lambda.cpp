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
bool keysEqualByLess(const K& a, const K& b) {
    return !(a < b) && !(b < a);
}

template <typename Elem, typename KeyFunc, typename ValueFunc>
std::vector<
    GroupInfo<
        typename std::result_of<KeyFunc(const Elem&)>::type,
        Elem,
        typename std::result_of<ValueFunc(const Elem&)>::type
    >
>
groupStats(const Elem* arr, std::size_t n, KeyFunc keyFunc, ValueFunc valueFunc)
{
    typedef typename std::result_of<KeyFunc(const Elem&)>::type KeyType;
    typedef typename std::result_of<ValueFunc(const Elem&)>::type SumType;
    typedef GroupInfo<KeyType, Elem, SumType> Group;

    std::vector<Group> groups;
    if (!arr || n == 0) return groups;

    for (std::size_t i = 0; i < n; ++i) {
        const Elem& x = arr[i];
        KeyType k = keyFunc(x);
        SumType v = valueFunc(x);

        bool found = false;
        for (std::size_t j = 0; j < groups.size(); ++j) {
            if (keysEqualByLess(groups[j].key, k)) {
                Group& g = groups[j];
                ++g.count;

                if (x < g.minElem) g.minElem = x;
                if (g.maxElem < x) g.maxElem = x;

                g.sum += v;
                found = true;
                break;
            }
        }

        if (!found) {
            Group g;
            g.key = k;
            g.count = 1;
            g.minElem = x;
            g.maxElem = x;
            g.sum = v;
            groups.push_back(g);
        }
    }

    for (std::size_t i = 0; i < groups.size(); ++i) {
        Group& g = groups[i];
        g.avg = g.count ? (static_cast<double>(g.sum) / static_cast<double>(g.count)) : 0.0;
    }

    return groups;
}

template <typename GI>
void printGroups(const std::vector<GI>& groups) {
    for (std::size_t i = 0; i < groups.size(); ++i) {
        const GI& g = groups[i];
        std::cout
            << g.key << ", "
            << g.count << ", "
            << g.minElem << ", "
            << g.maxElem << ", "
            << g.sum << ", "
            << g.avg
            << "\n";
    }
}

int main() {
    int a1[] = { 12, 25, 17, 30, 41, 52, 63, 70, 99, 100 };
    std::size_t n1 = sizeof(a1) / sizeof(a1[0]);

    auto g1 = groupStats(a1, n1,
        [](int x) { return x % 10; },
        [](int x) { return x; }
    );

    std::cout << "int groups:\n";
    printGroups(g1);
    std::cout << "\n";

    double a2[] = { -1.5, 0.0, 3.3, 10.0, 12.2, -5.0, 7.7, 100.0 };
    std::size_t n2 = sizeof(a2) / sizeof(a2[0]);

    auto g2 = groupStats(a2, n2,
        [](double x) {
            if (x < 0) return 0;
            if (x <= 10) return 1;
            return 2;
        },
        [](double x) { return x; }
    );

    std::cout << "double groups:\n";
    printGroups(g2);
    std::cout << "\n";

    std::string a3[] = { "who", "can", "sing", "that", "string", "check", "book" };
    std::size_t n3 = sizeof(a3) / sizeof(a3[0]);

    auto g3 = groupStats(a3, n3,
        [](const std::string& s) -> char { return s.empty() ? '\0' : s[0]; },
        [](const std::string& s) { return s.size(); }
    );

    std::cout << "string groups:\n";
    printGroups(g3);

    return 0;
}
