#include "ancient.h"

#include <bits/stdc++.h>
using namespace std;
using ll = long long;

std::pair<std::vector<int>, std::vector<int>> find_information(int N, int C) {
    assert(N == 2);
    std::vector<int> p(N), d(N);

    int l = -C, r = C;
    while (l + 1 < r) {
        int mid = (l + r) / 2;
        vector<long long> t(N);
        t[1] = mid;
        if (detect_convenience_values(t)[1] == 1) r = mid;
        else l = mid;
    }
    d[1] = -r;

    return std::make_pair(std::vector<int>(p.begin() + 1, p.end()), std::vector<int>(d.begin() + 1, d.end()));
}
