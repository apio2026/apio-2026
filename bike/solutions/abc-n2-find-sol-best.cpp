#include "bike.h"
#include <bits/stdc++.h>
using namespace std;
typedef long long ll;
typedef pair<int, int> pii;
#define pb push_back
#define all(a) a.begin(), a.end()
#define sz(a) ((int)a.size())

struct BIT {
    int n;
    vector <int> val;
    BIT (int _n) : n(_n + 5), val(n) {}
    void modify(int p, int v) {
        for (p++; p < n; p += p & (-p)) val[p] += v;
    }
    int query(int p) {
        int ans = 0;
        for (p++; p > 0; p -= p & (-p)) ans += val[p];
        return ans;
    }
};

pair<vector<int>, vector<long long>> find_rebalancing_strategy(int n, vector<int> A, vector<int> B, vector<int> U, vector<int> V) {
    vector <set <int>> adj(n);
    for (int i = 0; i < n - 1; ++i) {
        adj[U[i]].insert(V[i]);
        adj[V[i]].insert(U[i]);
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
                adj[u].erase(v);
                if (sz(adj[u]) == 1 && d[u] == 0) que.pb(u);
            }
            adj[v].clear();
        }
    }

    int rt = 0;
    while (dead[rt]) rt++;

    vector <ll> s = d;
    vector <int> p(n), in(n), out(n);
    int _t = 0;
    auto dfs = [&](auto self, int v, int pa) -> void {
        p[v] = pa, in[v] = _t++;
        for (int u : adj[v]) if (u != pa) {
            self(self, u, v);
            s[v] += s[u];
        }
        out[v] = _t;
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

    s = d, _t = 0;
    dfs(dfs, to, -1);

    BIT bit(n);
    auto modify = [&](int v, int op) {
        int x = s[v] < 0 ? 1 : -1;
        bit.modify(in[v], op * x);
        bit.modify(out[v], -op * x);
    };
    auto upd = [&](int v, int u, ll x) {
        if (p[v] == u) {
            modify(v, -1);
            s[v] -= x;
            modify(v, 1);
        } else {
            modify(u, -1);
            s[u] += x;
            modify(u, 1);
        }
    };
    auto get = [&](int u) {
        return 2 * (alive - 1) + bit.query(in[u]);
    };

    for (int i = 0; i < n; ++i) if (i != to && !dead[i]) {
        modify(i, 1);
    }

    vector <int> pts = {from};
    vector <ll> res = {0};
    for (int cur = ans - 1; ~cur; --cur) {
        int next = -1;
        ll take = 0; bool del = false;
        if (sz(adj[from]) == 1) {
            next = *adj[from].begin();
            if (a[from] >= b[from]) take = a[from] - b[from], del = true;
            else take = a[from];
        } else {
            for (int u : adj[from]) {
                upd(from, u, a[from]);
                int now = get(u);
                upd(from, u, -a[from]);
                if (now == cur) {
                    next = u;
                    take = a[from];
                    break;
                }
            }
        }

        assert(next != -1);

        if (del) {
            adj[next].erase(from);
            adj[from].clear();
            dead[from] = true;
            alive--;
        }
        pts.pb(next);
        res.back() -= take;
        res.pb(take);
        a[from] -= take;
        a[next] += take;
        upd(from, next, take);
        from = next;
    }

    return pair(pts, res);
}
