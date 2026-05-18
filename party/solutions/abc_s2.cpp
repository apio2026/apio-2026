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
    int p, r, n, tot;
    vector <int> cnt;
} // namespace

void init(int N, int K, int p, int r) {
    ::p = p, ::r = r, ::n = N;
    ::tot = K;
    cnt.resize(n);
}

int strategy(int b, int f, int s) {
    cnt[f]++;
    if (p != (f + 1) % n) return 0;
    return cnt[f] <= r ? tot : 0;
}

vector <int> guess(int N, int K, vector <int> F, vector <int> S) {
    vector <int> res(N);
    for (int i = 0; i < N * N; ++i) {
        res[F[i]] += S[i] == 0;
    }
    vector <int> ans(N);
    for (int i = 0; i < N; ++i) ans[res[i]] = (i + 1) % N;
    bug(ans);
    return ans;
}