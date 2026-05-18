#include <bits/stdc++.h>
#include "party.h"
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

namespace {
    // store global variables here
    int p, r, n, vis1, vis2;
    vector <int> cnt;
} // namespace

void init(int N, int K, int p, int r) {
    ::p = p, ::r = 0, ::n = N;
    cnt.resize(n);
}

int strategy(int b, int f, int s) {
    cnt[f]++;
    if (b < n + 1) {
        r += n - s;
        if (f == p || vis1 || s == 0) return 0;
        vis1 = 1;
        return 1;
    } else {
        if (p != (f + 1) % n || vis2) return 0;
        vis2 = 1;
        return n - r;
    }
}

vector <int> guess(int N, int K, vector <int> F, vector <int> S) {
    vector <int> ans(N, -1);
    int tot = 0;
    for (int i = 0; i < N; ++i) tot ^= i;
    for (int i = N + 1; i < N * N; ++i) if (S[i] != N) {
        ans[S[i]] = (F[i] + 1) % N;
        tot ^= ans[S[i]];
    }
    bug(ans);
    for (int i = 0; i < N; ++i) if (ans[i] == -1) {
        ans[i] = tot;
    }
    bug(ans);
    return ans;
}