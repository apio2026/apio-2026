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
    vector <int> ans(N - 1), in(N);
    for (int x : P) in[x] = 1;
    vector <vector <int>> c = {{0, 1, 2}, {0, 3, 4}, {1, 3, 2, 4}, {0}};
    for (int i = 0, cnt = 0, ptr = 0; i + 1 < N; ++i) {
        if (in[i]) {
            cnt++, ptr = 0;
        }
        int x = min(cnt, 6 - cnt);
        ans[i] = c[x][ptr];
        if (cnt < 3) ptr = (ptr + 1) % sz(c[x]);
        else ptr = (ptr + sz(c[x]) - 1) % sz(c[x]);
    }
    return ans;
}

std::vector<int> navigate(int K, int B, std::vector<int> C) {
    vector <vector <int>> ans = {{3, 0, 6, 1, 5},
                                 {6, 9, 0, 2, 4},
                                 {0, 6, 9, 4, 2},
                                 {5, 4, 2, 9, 1},
                                 {1, 2, 4, 5, 9}};

    bug(C);
    return {ans[C[0]][C[1]], ans[C[1]][C[0]]};
}
