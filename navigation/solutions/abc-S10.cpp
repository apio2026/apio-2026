#include <bits/stdc++.h>
#include "navigation.h"
using namespace std;
typedef long long ll;
typedef pair<int, int> pii;
#define pb push_back
#define all(a) a.begin(), a.end()
#define sz(a) ((int)a.size())
#ifdef Doludu
template <typename T>
ostream& operator << (ostream &o, vector <T> vec) {
    o << "{"; int f = 0;
    for (T i : vec) o << (f++ ? " " : "") << i;
    return o << "}";
}
void bug__(int c, auto ...a) {
    cerr << "\e[1;" << c << "m";
    (..., (cerr << a << " "));
    cerr << "\e[0m" << endl;
}
#define bug_(c, x...) bug__(c, __LINE__, "[" + string(#x) + "]", x)
#define bug(x...) bug_(32, x)
#define bugv(x...) bug_(36, vector(x))
#define safe bug_(33, "safe")
#else
#define bug(x...) void(0)
#define bugv(x...) void(0)
#define safe void(0)
#endif

std::vector<int> construct_network(int N, int K, int B, std::vector<int> U,
                                   std::vector<int> V, std::vector<int> P) {
    vector <vector <int>> adj(N);
    for (int i = 0; i < N - 1; ++i) {
        adj[U[i]].pb(i);
        adj[V[i]].pb(i);
    }
    vector <int> in(N);
    vector <int> s(N, 0);
    for (int v : P) s[v] = in[v] = 1;
    auto dfs = [&](auto self, int v, int pa) -> void {
        for (int id : adj[v]) {
            int u = V[id] ^ U[id] ^ v;
            if (u == pa) continue;
            self(self, u, v);
            s[v] += s[u];
        }
    };
    int rt = P[0];
    dfs(dfs, rt, -1);

    vector <int> ans(N - 1, -1);
    auto dfs2 = [&](auto self, int v, int pa, int lst) -> void {
        for (int id : adj[v]) {
            int u = V[id] ^ U[id] ^ v;
            if (u == pa) continue;
            ans[id] = min(s[u], 6 - s[u]) * 3;
            if (lst != -1 && ans[id] != 9) {
                ans[id] += (ans[lst] + (s[u] < 3 ? 1 : 2)) % 3;
            }
            self(self, u, v, id);
        }
    };
    dfs2(dfs2, rt, -1, -1);
    bug(ans);
    return ans;
}

std::vector<int> navigate(int K, int B, std::vector<int> C) {
    vector <int> s(sz(C));
    int cnt = 0;
    for (int x : C) cnt += x >= 3;
    bug(C);
    if (cnt == 0 || cnt == 2) {
        int vis = 0, mx = ranges::max(C);
        for (int x : C) if (mx / 3 == x / 3) {
            vis |= 1 << (x % 3);
        }

        if (mx == 9) {
            for (int i = 0; i < sz(C); ++i) {
                if (C[i] == 9) s[i] = 3;
            }
        } else {
            int goal = -1;
            if (vis == 0b011) goal = 0;
            else if (vis == 0b110) goal = 1;
            else if (vis == 0b101) goal = 2;
            else assert(false);

            for (int i = 0; i < sz(C); ++i) {
                if (C[i] % 3 == goal) s[i] = 6 - mx / 3;
                else if (C[i] >= 3) s[i] = mx / 3;
            }
        }
        bug(s);
    } else {
        assert(cnt >= 3);
        int sum = 0;
        for (int i = 0; i < sz(C); ++i) {
            s[i] = C[i] / 3;
            sum += s[i];
        }
        bug(s);
        if (sum == 4) {
            *find(all(s), 2) = 4;
        } else {
            assert(sum == 6);
        }
    }
    return s;
}
