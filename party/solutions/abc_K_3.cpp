#include "party.h"
#include <bits/stdc++.h>
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
    int n, k, p, r, vis1, m;
} // namespace

void init(int n, int k, int p, int r) {
    ::n = n, ::k = k, ::p = p;
    ::m = n + (n - 1) / 3 + 1;
    return;
}

int strategy(int b, int f, int s) {
    if (b < m) {
        r += k - s;
        if (f == p) return 0;
        if (!vis1 && s > 0) {
            vis1 = 1;
            return 1;
        }
        return 0;
    }
    if (f == (p + 1) % n) {
        int mn = min(r, s);
        r -= mn;
        return mn;
    } else {
        return 0;
    }
}

std::vector<int> guess(int n, int k, std::vector<int> f, std::vector<int> s) {
    m = n + (n - 1) / 3 + 1;
    vector <int> res(n, -1);
    int tot = 0;
    for (int i = 0; i < n; ++i) tot ^= i;
    vector <int> cnt(n, 0);
    for (int i = 0; i < m; ++i) {
        cnt[f[i]]++;
    }
    int mx = max_element(all(cnt)) - cnt.begin();
    int unknown = (mx + n - 1) % n;
    for (int i = 0; i < n; ++i) if (i != unknown) {
        int c = 0;
        for (int j = m; j < n * n; ++j) if (f[j] == (i + 1) % n) {
            c += k - s[j];
        }
        res[c] = i;
        tot ^= i;
    }
    *find(all(res), -1) = tot;
    return res;
}
