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
    int p, r, n, vis, tot;
    vector <int> cnt;
} // namespace

void init(int N, int K, int p, int r) {
    ::p = p, ::r = 0, ::n = N;
    ::tot = K;
    cnt.resize(n);
}

int strategy(int b, int f, int s) {
    cnt[f]++;
    if (s == 0) vis = 1;
    if (f == p) return 0;
    if (cnt[f] == 1) return tot;
    return vis * tot;
}

vector <int> guess(int N, int K, vector <int> F, vector <int> S) {
    vector <int> ans(N);
    if (count(all(S), 0) == 2) {
        for (int i = N * N - 1; ~i; --i) {
            if (S[i] == 0) {
                ans[0] = F[i] ^ 1;
                break;
            }
        }
    } else {
        int x = find(all(S), K) - S.begin();
        ans[0] = F[x] ^ 1;
    }
    ans[1] = 1 ^ ans[0];
    bug(ans);
    return ans;
}