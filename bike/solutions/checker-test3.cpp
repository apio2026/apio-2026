#include "bike.h"

#include <bits/stdc++.h>
using namespace std;

#define pb emplace_back
using pii = pair<int, int>;
using ll = long long;
using pll = pair<ll, ll>;

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

    vector<bool> done(N);
    int st = -1, ed = -1, cost = -1;
    int remain_vertex = N;
    {
        int rt = -1;
        for (int i = 0; i < N; i++) {
            if (A[i] != B[i]) {
                rt = i;
                break;
            }
        }

        vector<ll> subtree(N);
        auto dfs1 = [&](auto self, int now, int p) -> pair<pii, pii> {
            pii max_up(0, now);
            pii max_down(0, now);
            subtree[now] = A[now] - B[now];
            bool bad = false;
            for (int i : g[now]) {
                if (i == p) continue;
                auto [up, down] = self(self, i, now);
                int tmp = up.first + max_down.first;
                if (tmp > cost) cost = tmp, st = up.second, ed = max_down.second;
                tmp = down.first + max_up.first;
                if (tmp > cost) cost = tmp, st = max_up.second, ed = down.second;
                max_up = max(max_up, up);
                max_down = max(max_down, down);
                subtree[now] += subtree[i];
                bad = bad || !done[i];
            }
            if (!bad && A[now] == B[now]) {
                done[now] = true;
                remain_vertex--;
                max_up = pii(-1, now);
                max_down = pii(-1, now);
            }
            else {
                if (subtree[now] >= 0) max_up.first++;
                else max_up.first--;
                if (subtree[now] <= 0) max_down.first++;
                else max_down.first--;
            }
            return make_pair(max_up, max_down);
        };
        dfs1(dfs1, rt, rt);
    }

    vector<long long> subtree_diff(N);
    vector<int> parent(N);
    vector<int> path;
    auto dfs1 = [&](auto self, int now, int p) -> bool {
        parent[now] = p;
        subtree_diff[now] = A[now] - B[now];
        bool ok = now == ed;
        for (int i : g[now]) {
            if (i == p) continue;
            ok = self(self, i, now) || ok;
            subtree_diff[now] += subtree_diff[i];
        }
        if (ok) path.emplace_back(now);
        return ok;
    };
    dfs1(dfs1, st, st);
    reverse(path.begin(), path.end());
    vector<int> path_index(N, -1);
    for (int i = 0; i < ssize(path); i++)
        path_index[path[i]] = i;

    cost = 2 * (remain_vertex - 1) - cost;
    vector<int> X(cost + 1, INT_MIN);
    vector<long long> Y(cost + 1, LLONG_MIN);
    
    return {X, Y};
}
