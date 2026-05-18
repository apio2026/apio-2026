#include "bike.h"
#include <bits/stdc++.h>
using namespace std;
using ll=long long;

std::pair<std::vector<int>, std::vector<long long>>
        find_rebalancing_strategy(int N, 
                                  std::vector<int> _A, 
                                  std::vector<int> B, 
                                  std::vector<int> U,
                                  std::vector<int> V) {
    vector<long long> A(_A.begin(), _A.end());
    vector<ll> c(N);
    for (int i = 0; i < N; ++i) c[i] = A[i] - B[i];
    vector<vector<int>> g(N);
    for(int i = 0; i < N - 1; ++i) {
        int u = U[i], v = V[i];
        g[u].push_back(v);
        g[v].push_back(u);
    }
    vector<int> deg(N);
    for(int i = 0; i < N; ++i) deg[i] = g[i].size();

    vector<bool> deleted(N, false);
    queue<int> que;
    for(int i = 0; i < N; ++i) {
        if(c[i] == 0 && deg[i] == 1) {
            deleted[i] = true;
            que.emplace(i);
        }
    }
    while(!que.empty()) {
        int i = que.front(); que.pop();
        for(int v : g[i]) {
            if(deleted[v]) continue;
            deg[v]--;
            if(c[v] == 0 && deg[v] == 1) {
                deleted[v] = true;
                que.emplace(v);
            }
        }
    }

    int n = 0;
    for(int i = 0; i < N; ++i) if(!deleted[i]) n++;
    int r = 0;
    while(deleted[r]) r++;

    vector<ll> s = c;
    pair<int, int> res(r, r);
    vector<pair<int, int>> dpd(N), dpu(N);
    auto dfs = [&](auto&& self, int u, int p) -> void {
        dpd[u] = {0, u};
        for(int v : g[u]) {
            if(v == p || deleted[v]) continue;
            self(self, v, u);
            s[u] += s[v];
            dpd[u] = max(dpd[u], {dpd[v].first + (s[v] > 0 ? -1 : 1), dpd[v].second});
        }
    }; dfs(dfs, r, -1);

    auto dfs2 = [&](auto && self, int u, int p) -> void {
        vector<int> ch;
        for(int v : g[u]) {
            if(v == p || deleted[v]) continue;
            ch.emplace_back(v);
        }
        if(!ch.size()) return;
        vector<pair<int, int>> l(ch.size()), r(ch.size());
        l[0] = r.back() = {0, u};
        for(int i = 0; i < (int)ch.size() - 1; ++i) {
            int v = ch[i];
            l[i + 1] = max(l[i], {dpd[v].first + (s[v] > 0 ? -1 : 1), dpd[v].second});
        }
        for(int i = (int)ch.size() - 1; i > 0; --i) {
            int v = ch[i];
            r[i - 1] = max(r[i], {dpd[v].first + (s[v] > 0 ? -1 : 1), dpd[v].second});
        }
        for(int i = 0; i < ch.size(); ++i) {
            int v = ch[i];
            dpu[v] = max({dpu[u], l[i], r[i]});
            dpu[v].first += (-s[v] > 0 ? -1 : 1);
        }
        for(int v : ch) {
            self(self, v, u);
        }
    }; dfs2(dfs2, r, -1);

    int ma = -1;
    for(int i = 0; i < N; ++i) {
        if(deleted[i]) continue;
        auto p = max(dpd[i], dpu[i]);
        if(p.first > ma) {
            ma = p.first;
            res = {i, p.second};
        }
    }

    for(int i = 0; i < N; ++i) s[i] = c[i];
    vector<int> par(N, -1);
    auto dfs3 = [&](auto&& self, int u, int p) -> void {
        par[u] = p;
        for(int v : g[u]) {
            if(v == p || deleted[v]) continue;
            self(self, v, u);
            s[u] += s[v];
        }
    }; dfs3(dfs3, res.first, -1);
    vector<bool> on_path(N, false);
    vector<int> path;
    for(int x = res.second; x != -1; x = par[x]) {
        on_path[x] = true;
        path.push_back(x);
    }
    reverse(path.begin(), path.end());

    vector<int> X;
    auto clean = [&](auto&& self, int u) -> void {
        X.push_back(u);
        for(int v : g[u]) if(!deleted[v] && v != par[u] && s[v] >= 0) {
            self(self, v);
            X.push_back(u);
        }
        for(int v : g[u]) if(!deleted[v] && v != par[u] && s[v] < 0) {
            self(self, v);
            X.push_back(u);
        }
    };

    int l = 0;
    while(l < path.size()) {
        int r = l;
        while(r + 1 < path.size() && s[path[r + 1]] > 0) r++;
        for(int i = l; i <= r; ++i) {
            X.push_back(path[i]);
            for(int v : g[path[i]]) {
                if(on_path[v] || deleted[v] || s[v] <= 0) continue;
                clean(clean, v);
                X.push_back(path[i]);
            }
        }
        for(int i = r - 1; i >= l; --i) {
            X.push_back(path[i]);
        }
        for(int i = l; i <= r; ++i) {
            if(i != l) X.push_back(path[i]);
            for(int v : g[path[i]]) {
                if(on_path[v] || deleted[v] || s[v] > 0) continue;
                clean(clean, v);
                X.push_back(path[i]);
            }
        }
        l = r + 1;
    }

    vector<ll> Y;
    vector<bool> last_visit(X.size()), flag(N, true);
    for(int i = X.size() - 1; i >= 0; --i) {
        int u = X[i];
        if(flag[u]) {
            flag[u] = false;
            last_visit[i] = true;
        }
    }

    for(int i = 0; i < X.size() - 1; ++i) {
        ll res = A[X[i]];
        if(last_visit[i]) res -= B[X[i]];
        Y.push_back(res);
        A[X[i]] -= res;
        A[X[i + 1]] += res;
    }

    vector<ll> outputY = {0};
    for (auto i : Y) {
        outputY.back() -= i;
        outputY.emplace_back(i);
    }
    return {X, outputY};
}
