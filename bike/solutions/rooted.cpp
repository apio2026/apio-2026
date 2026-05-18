#include "bike.h"

#include <bits/stdc++.h>
using namespace std;

std::pair<std::vector<int>, std::vector<long long>>
        find_rebalancing_strategy(int N, 
                                  std::vector<int> A, 
                                  std::vector<int> B, 
                                  std::vector<int> U,
                                  std::vector<int> V) {
    
    vector<vector<int>> g(N);
    for (int i = 0; i < N - 1; i++) {
        int u = U[i], v = V[i];
        g[u].emplace_back(v);
        g[v].emplace_back(u);
    }

    int rt = -1;
    for (int i = 0; i < N; i++) {
        if (A[i]) {
            rt = i;
            break;
        }
    }

    vector<int> X;
    vector<long long> Y;

    vector<int> hgt(N), nxt(N, -1);
    vector<bool> done(N);
    auto dfs1 = [&](auto self, int now, int p) -> void {
        if (A[now] == B[now]) done[now] = true;
        for (int i : g[now]) {
            if (i == p) continue;
            self(self, i, now);
            if (!done[i]) {
                done[now] = false;
                hgt[now] = max(hgt[now], hgt[i] + 1);
                if (nxt[now] == -1 || hgt[nxt[now]] < hgt[i]) nxt[now] = i;
            }
        }
    };
    dfs1(dfs1, rt, rt);

    auto walk = [&](int v, long long amount) {
        if (X.empty() || X.back() != v) X.emplace_back(v), Y.emplace_back(0);
        Y.back() += amount;
        A[v] += amount;
        assert(A[v] >= 0);
    };

    auto dfs2 = [&](auto self, int now, int p, bool noret) -> void {
        walk(now, -A[now]);
        for (int i : g[now]) {
            if (i == p || (noret && i == nxt[now]) || done[i]) continue;
            walk(now, 0);
            self(self, i, now, false);
        }
        walk(now, B[now]);
        if (nxt[now] != -1 && noret) {
            self(self, nxt[now], now, true);
        }
    };
    dfs2(dfs2, rt, rt, true);

    return {X, Y};
}
