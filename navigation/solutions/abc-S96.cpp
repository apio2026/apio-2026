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
    vector <int> s(N, 0);
    for (int i = 1; i < K; ++i) s[P[i]] |= 1 << (i - 1);

    vector <int> ans(N - 1);
    auto dfs = [&](auto self, int v, int pa, int d) -> void {
        for (int id : adj[v]) {
            int u = U[id] ^ V[id] ^ v;
            if (u == pa) continue;
            self(self, u, v, (d + 1) % 3);
            ans[id] = s[u] + (d << 5);
            s[v] |= s[u];
        }
    };
    dfs(dfs, P[0], -1, 0);

    return ans;
}

std::vector<int> navigate(int K, int B, std::vector<int> C) {
    bug(C);
    vector <int> s(sz(C));
    int vis = 0;
    for (int &x : C) vis |= 1 << (x >> 5);
    int goal = -1;
    if (vis == 0b110) goal = 1;
    else if (vis == 0b101) goal = 2;
    else if (vis == 0b011) goal = 0;
    else assert(false);
    for (int i = 0; i < sz(C); ++i) {
        if ((C[i] >> 5) == goal) {
            s[i] = 6 - __builtin_popcount(C[i] & 31);
        } else {
            s[i] = __builtin_popcount(C[i] & 31);
        }
    }
    return s;
}
