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
    int rt = -1;
    auto dfs = [&](auto self, int v, int pa) -> void {
        int good = 1;
        for (int id : adj[v]) {
            int u = V[id] ^ U[id] ^ v;
            if (u == pa) continue;
            self(self, u, v);
            s[v] += s[u];
            if (s[u] > 3) good = 0;
        }
        if (s[v] < 3) good = 0;
        if (good) rt = v;
    };
    dfs(dfs, 0, -1);
    bug(rt);

    s.assign(N, 0);
    for (int v : P) s[v] = 1;
    dfs(dfs, rt, -1);

    vector <int> ans(N - 1, -1);
    auto dfs2 = [&](auto self, int v, int pa, int lst) -> void {
        if (s[v] == 0) {
            for (int id : adj[v]) if (id != lst) {
                int u = V[id] ^ U[id] ^ v;
                if (ans[lst] == 2) ans[id] = 4;
                else if (ans[lst] == 4) ans[id] = 5;
                else ans[id] = 2;
                self(self, u, v, id);
            }
            return;
        }
        if (in[v]) {
            for (int id : adj[v]) if (id != lst) {
                int u = V[id] ^ U[id] ^ v;
                ans[id] = s[u] == 0 ? 5 : 0;
                self(self, u, v, id);
            }
            return;
        }
        vector <int> nxt;
        for (int id : adj[v]) if (id != lst) {
            int u = V[id] ^ U[id] ^ v;
            if (s[u] != 0) nxt.pb(id);
            else {
                ans[id] = 5;
                self(self, u, v, id);
            }
        }
        bug(v, s[v], nxt);
        if (sz(nxt) == 1) {
            int id = nxt[0];
            int u = V[id] ^ U[id] ^ v;
            if (s[v] == 3) ans[id] = 0;
            else if (s[v] == 2) {
                if (ans[lst] == 0) ans[id] = 3;
                else if (ans[lst] == 3) ans[id] = 4;
                else ans[id] = 0;
            } else {
                if (ans[lst] == 0) ans[id] = 1;
                else if (ans[lst] == 1) ans[id] = 2;
                else ans[id] = 0;
            }
            self(self, u, v, id);
        } else {
            for (int id : nxt) {
                int u = V[id] ^ U[id] ^ v;
                assert(s[u] == 1 || s[u] == 2);

                if (s[u] == 1) ans[id] = 1;
                else if (s[u] == 2) ans[id] = 3;
                else assert(false);
                self(self, u, v, id);
            }
        }
    };

    for (int id : adj[rt]) {
        int u = V[id] ^ U[id] ^ rt;
        bug(u, s[u]);
        if (s[u] == 0) ans[id] = 5;
        else if (s[u] == 1) ans[id] = 1;
        else if (s[u] == 2) ans[id] = 3;
        else ans[id] = 0;
        dfs2(dfs2, u, rt, id);
    }

    bug(ans);

    return ans;
}

std::vector<int> navigate(int K, int B, std::vector<int> C) {
    vector <int> s(sz(C));
    int mask = 0;
    for (int i = 0; i < sz(C); ++i) {
        mask |= 1 << C[i];
    }
    if (mask == 0b110000) {
        int pos = find(all(C), 4) - C.begin();
        s[pos] = 6;
    } else if (mask == 0b100100) {
        int pos = find(all(C), 5) - C.begin();
        s[pos] = 6;
    } else if (mask == 0b010100) {
        int pos = find(all(C), 2) - C.begin();
        s[pos] = 6;
    } else {
        vector <int> tmp;
        for (int i = 0; i < sz(C); ++i) {
            if (C[i] != 5) {
                tmp.pb(i);
            }
        }
        bug(tmp);
        if (sz(tmp) == 2) {
            if (C[tmp[0]] > C[tmp[1]]) swap(tmp[0], tmp[1]);
            if (C[tmp[0]] == 0 && C[tmp[1]] == 0) {
                s[tmp[0]] = s[tmp[1]] = 3;
            } else if (C[tmp[0]] == 0 && C[tmp[1]] == 1) {
                s[tmp[0]] = 5, s[tmp[1]] = 1;
            } else if (C[tmp[0]] == 1 && C[tmp[1]] == 2) {
                s[tmp[0]] = 5, s[tmp[1]] = 1;
            } else if (C[tmp[0]] == 0 && C[tmp[1]] == 2) {
                s[tmp[0]] = 1, s[tmp[1]] = 5;
            } else if (C[tmp[0]] == 0 && C[tmp[1]] == 3) {
                s[tmp[0]] = 4, s[tmp[1]] = 2;
            } else if (C[tmp[0]] == 3 && C[tmp[1]] == 4) {
                s[tmp[0]] = 4, s[tmp[1]] = 2;
            } else if (C[tmp[0]] == 0 && C[tmp[1]] == 4) {
                s[tmp[0]] = 2, s[tmp[1]] = 4;
            }
        } else {
            int tot = 6;
            for (int id : tmp) {
                if (C[id] == 1) s[id] = 1, tot -= 1;
                else if (C[id] == 3) s[id] = 2, tot -= 2;
            }
            for (int id : tmp) {
                if (C[id] != 1 && C[id] != 3) {
                    s[id] = tot;
                    tot = 0;
                }
            }
            if (tot == 2) {
                for (int &x : s) {
                    if (x == 2) x = 4;
                }
            }
        }
    }

    bug(C);
    return s;
}
