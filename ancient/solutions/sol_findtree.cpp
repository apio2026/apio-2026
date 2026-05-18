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

    for (int b = 0; b <= __lg(N); b++) {
        vector<ll> t(N);
        int cnt = 0;
        for (int i = 0; i < N; i++) {
            if (!(1 << b & i)) t[i] = ++cnt;
        }
        for (int i = N - 1; i >= 0; i--) {
            if ((1 << b & i)) t[i] = ++cnt;
        }
        auto c = detect_convenience_values(t);
        for (int i = 0; i < N; i++) {
            int tmp = c[i] - (!!(1 << b & i) * child[i]);
            if (tmp > 0);
            else p[i] |= 1 << b;
        }
    }


    return std::make_pair(std::vector<int>(p.begin() + 1, p.end()), std::vector<int>(d.begin() + 1, d.end()));
}
