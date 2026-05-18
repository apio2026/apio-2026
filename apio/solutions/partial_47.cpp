#include "apio.h"
#include <bits/stdc++.h>
using namespace std;

namespace {
    const int B = 22;
    int vis[1 << B], c[B];
    vector <int> vec;
    int dp[48];
    bool inited = false;

    bool check(int i, int r, int op) {
        if (op == -1 && vis[i] != -1) {
            return true;
        }
        if (op >= 0) {
            vis[i] = op;
        }
        if (r == 0) {
            return false;
        }
        for (int j = 1; j < B; ++j) {
            if (i >> j & 1 && ~i >> j - 1 & 1) {
                if (check(i ^ (1 << j) ^ (1 << j - 1), r - 1, op)) {
                    return true;
                }
            }
        }
        return false;
    }
    string encode(int x, int len) {
        int id = vec[x];
        assert(id < (1 << len));
        string s;
        for (int i = 0; i < len; ++i) {
            s += "OI"[id >> i & 1];
        }
        return s;
    }
    int decode(string s) {
        int x = 0;
        for (int i = 0; i < (int)s.size(); ++i) {
            if (s[i] == 'I') {
                x |= 1 << i;
            }
        }
        return vis[x];
    }
}

void init() {
    fill(vis, vis + (1 << B), -1);
    int cnt = 0;
    for (int i = 0; i < (1 << B); ++i) {
        if (check(i, 3, -1)) {
            continue;
        }
        check(i, 3, cnt);
        cnt++;
        vec.push_back(i);
        for (int j = 0; j < B; ++j) if (i < (1 << j + 1)) {
            c[j + 1]++;
        }
    }
    for (int i = 1; i <= B; ++i) {
        dp[i * 2 + 3] = c[i] * c[i];
        if (i) {
            dp[i * 2 - 1 + 3] = c[i - 1] * c[i];
        }
    }
}

string Alice(int x, int k) {
    if (!inited) init(), inited = true;
    int len = 0;
    while (x >= dp[len]) {
        x -= dp[len], len++;
    }
    int llen = (len - 3) / 2, rlen = (len - 3 + 1) / 2;
    return encode(x % c[llen], llen) + "III" + encode(x / c[llen], rlen);
}

int Bob(string s, int k) {
    if (!inited) init(), inited = true;
    int len = (int)s.size();
    int ans = 0;
    for (int i = 0; i < len; ++i) {
        ans += dp[i];
    }
    int llen = (len - 3) / 2, rlen = (len - 3 + 1) / 2;
    for (int i = llen; i < llen + 3; ++i) if (s[i] == 'O') {
        int j = llen - 1;
        while (s[j] == 'O') {
            j--;
        }
        swap(s[j], s[i]);
    }
    assert(s.substr(llen, 3) == "III");
    return ans + decode(s.substr(0, llen)) + decode(s.substr(llen + 3)) * c[llen];
}