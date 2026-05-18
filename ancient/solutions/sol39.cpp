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
        for (int i = 0; i < N; i++) {
            t[i] = t[i] * C;
        }
        auto c = detect_convenience_values(t);
        for (int i = 0; i < N; i++) {
            int tmp = c[i] - (!!(1 << b & i) * child[i]);
            if (tmp > 0);
            else p[i] |= 1 << b;
        }
    }

    vector<ll> L(N, -C), R(N, C);
    while (true) {
        bool done = true;
        vector<ll> t(N);
        for (int i = 1; i < N; i++) {
            t[i] = (L[i] + R[i]) / 2;
            if (L[i] + 1 >= R[i])
                continue;
            done = false;
        }
        if (done) break;
        for (int i = 1; i < N; i++)
            t[i] = t[p[i]] + t[i];
        auto c = detect_convenience_values(t);
        for (int i = N - 1; i >= 1; i--) {
            int mid = (L[i] + R[i]) / 2;
            if (!c[i]) {
                L[i] = mid;
                c[p[i]]--;
            }
            else {
                R[i] = mid;
            }
        }
    }
    for (int i = 1; i < N; i++) {
        d[i] = -R[i];
    }
    for (int i = 1; i < N; i++) {
        d[i] = d[p[i]] + d[i];
    }

    return std::make_pair(std::vector<int>(p.begin() + 1, p.end()), std::vector<int>(d.begin() + 1, d.end()));
}
