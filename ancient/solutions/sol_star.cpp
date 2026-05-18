#include "ancient.h"

#include <bits/stdc++.h>
using namespace std;
using ll = long long;

std::pair<std::vector<int>, std::vector<int>> find_information(int N, int C) {
    std::vector<int> p(N), d(N);

    vector<int> L(N, -C), R(N, C);

    while (true) {
        bool done = true;
        vector<ll> t(N);
        for (int i = 1; i < N; i++) {
            t[i] = (L[i] + R[i]) / 2;
            if (L[i] + 1 < R[i]) done = false;
        }
        if (done) break;
        auto c = detect_convenience_values(t);
        for (int i = 1; i < N; i++) {
            int mid = (L[i] + R[i]) / 2;
            if (c[i]) R[i] = mid;
            else L[i] = mid;
        }
    }

    for (int i = 1; i < N; i++)
        d[i] = -R[i];

    return std::make_pair(std::vector<int>(p.begin() + 1, p.end()), std::vector<int>(d.begin() + 1, d.end()));
}
