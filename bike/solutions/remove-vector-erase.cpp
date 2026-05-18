#include "bike.h"
#include <bits/stdc++.h>
using namespace std;
typedef long long ll;
typedef pair<int, int> pii;
#define pb push_back
#define all(a) a.begin(), a.end()
#define sz(a) ((int)a.size())

pair<vector<int>, vector<long long>> find_rebalancing_strategy(int n, vector<int> A, vector<int> B, vector<int> U, vector<int> V) {
    vector <vector <int>> adj(n);
    for (int i = 0; i < n - 1; ++i) {
        adj[U[i]].pb(V[i]);
        adj[V[i]].pb(U[i]);
    }
    vector <ll> a(all(A)), b(all(B)), d(n);
    for (int i = 0; i < n; ++i) d[i] = a[i] - b[i];

    vector <int> dead(n, 0);
    {
        vector <int> que;
        for (int i = 0; i < n; ++i) {
            if (sz(adj[i]) == 1 && d[i] == 0) que.pb(i);
        }
        for (int i = 0; i < sz(que); ++i) {
            int v = que[i];
            dead[v] = 1;
            for (int u : adj[v]) {
                adj[u].erase(find(all(adj[u]), v));
                if (sz(adj[u]) == 1 && d[u] == 0) que.pb(u);
            }
            adj[v].clear();
        }
        for (int i = 0; i < n; ++i) {
            if (dead[i]) adj[i].clear();
            else {
                vector <int> tmp;
                for (int j : adj[i]) if (!dead[j]) {
                    tmp.pb(j);
                }
                adj[i] = tmp;
            }
        }
    }

    int rt = 0;
    while (dead[rt]) rt++;

    vector <ll> s = d;
    vector <int> p(n);
    auto dfs = [&](auto self, int v, int pa) -> void {
        p[v] = pa;
        for (int u : adj[v]) if (u != pa) {
            self(self, u, v);
            s[v] += s[u];
        }
    };

    dfs(dfs, rt, -1);

    const int INF = 5e8;
    array <int, 3> best{INF, 0, 0};
    auto dfs2 = [&](auto self, int v, int pa) -> pair<pii, pii> {
        pii best_up(0, v), best_down(0, v);
        for (int u : adj[v]) if (u != pa) {
            auto [next_up, next_down] = self(self, u, v);
            if (s[u] < 0) next_up.first++, next_down.first--;
            else if (s[u] > 0) next_up.first--, next_down.first++;
            else next_up.first--, next_down.first--;
            best = min(best, {best_up.first + next_down.first, best_up.second, next_down.second});
            best = min(best, {next_up.first + best_down.first, next_up.second, best_down.second});
            best_up = min(best_up, next_up);
            best_down = min(best_down, next_down);
        }
        return pair(best_up, best_down);
    };

    dfs2(dfs2, rt, -1);

    int alive = count(all(dead), 0);
    int ans = 2 * (alive - 1) + best[0];
    int from = best[1], to = best[2];

    s = d;
    dfs(dfs, from, -1);
    vector <int> path;
    for (int i = to; i != -1; i = p[i]) path.pb(i);
    reverse(all(path));

    for (int i = 0; i < n; ++i) {
        sort(all(adj[i]), [&](int x, int y) {
            return s[x] > s[y];
        });
    }

    vector <int> pts = {from};
    vector <ll> res = {0};

    int cur = from;
    auto add = [&](int v, int u, bool take_all) {
        assert(cur == v);
        ll w = take_all ? a[v] : a[v] - b[v];
        assert(w >= 0);
        pts.pb(u);
        res.back() -= w;
        res.pb(w);
        a[v] -= w;
        a[u] += w;
        cur = u;
    };
    
    auto dfs3 = [&](auto self, int v, int pa) -> void {
        for (int u : adj[v]) if (u != pa) {
            add(v, u, true);
            self(self, u, v);
        }
        add(v, pa, false);
    };
    auto gao = [&](int i, int type) -> void {
        int nxt = i + 1 < sz(path) ? path[i + 1] : -1;
        int prv = i ? path[i - 1] : -1;
        for (int u : adj[path[i]]) if (u != nxt && u != prv) {
            if (type == 1 && s[u] <= 0) continue;
            if (type == 2 && s[u] > 0) continue;
            add(path[i], u, true);
            dfs3(dfs3, u, path[i]);
        }
    };

    for (int i = 0, j = 0; i + 1 < sz(path); i = j) {
        while (j + 1 < sz(path) && s[path[j + 1]] > 0) {
            add(path[j], path[j + 1], true);
            j++;
        }
        for (int ii = j; ii > i; --ii) {
            gao(ii, 1);
            add(path[ii], path[ii - 1], true);
        }
        gao(i, 1);
        for (int ii = i; ii < j; ++ii) {
            gao(ii, 2);
            add(path[ii], path[ii + 1], false);
        }
        gao(j, 2);
        assert(j + 1 < sz(path));
        add(path[j], path[j + 1], false);
        j++;
    }
    gao(sz(path) - 1, 1);
    gao(sz(path) - 1, 2);

    assert(ans == sz(pts) - 1);

    return pair(pts, res);
}
