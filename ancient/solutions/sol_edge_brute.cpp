#include "ancient.h"

#include <bits/stdc++.h>
using namespace std;
using ll = long long;

std::pair<std::vector<int>, std::vector<int>> find_information(int N, int C) {
    assert(N == 2 && C == 350);
    std::vector<int> p(N), d(N);

    for (int i = -C + 1; i <= C; i++) {
        vector<long long> t(N);
        t[1] = i;
        if (detect_convenience_values(t)[1] == 1) {
            d[1] = -i;
            break;
        }
    }

    return std::make_pair(std::vector<int>(p.begin() + 1, p.end()), std::vector<int>(d.begin() + 1, d.end()));
}
