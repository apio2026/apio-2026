#include "ancient.h"

#include <bits/stdc++.h>
using namespace std;
using ll = long long;

std::pair<std::vector<int>, std::vector<int>> find_information(int N, int C) {
    std::vector<int> p(N), d(N);

    vector<int> child;
    {
        vector<ll> t(N);
        for (int i = 0; i < N; i++)
            t[i] = (ll)(N - i) * C;
        child = detect_convenience_values(t);
    }

    for (int i = 0; i + 1 < N; i++) {
        vector<ll> t(N);
        for (int j = 0; j < N; j++)
            t[j] = N - j;
        t[i] = -1;
        auto c = detect_convenience_values(t);
        for (int j = i + 1; j < N; j++) {
            if (c[j] > child[j]) p[j] = i;
        }
    }

    return std::make_pair(std::vector<int>(p.begin() + 1, p.end()), std::vector<int>(d.begin() + 1, d.end()));
}
