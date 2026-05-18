#include "apio.h"
#include <bits/stdc++.h>
using namespace std;

namespace {
    long long dp[100];
}

string Alice(int x, int k) {
    dp[0] = 1;
    for (int i = 1; i < 70; ++i) {
        dp[i] = dp[i - 1];
        if (i >= 2) {
            dp[i] += dp[i - 2];
        }
    }
    assert(k == 1);
    int len = 0;
    while (x >= dp[len]) {
        x -= dp[len], len++;
    }
    string res;
    for (int i = 0; i < len; ) {
        if (x < dp[len - i - 1]) {
            res += "O";
            i++;
        } else {
            x -= dp[len - i - 1];
            res += "II";
            i += 2;
        }
    }
    return res;
}

int Bob(string s, int k) {
    dp[0] = 1;
    for (int i = 1; i < 70; ++i) {
        dp[i] = dp[i - 1];
        if (i >= 2) {
            dp[i] += dp[i - 2];
        }
    }
    int len = (int)s.size();
    int ans = 0;
    for (int i = 0; i < len; ++i) {
        ans += dp[i];
    }
    for (int i = 0; i < len; ) {
        if (s[i] == 'I') {
            assert(i + 2 <= len);
            if (s.substr(i, 2) != "II") {
                int j = find(s.begin() + i, s.end(), 'O') - s.begin();
                swap(s[i], s[j]);
            }
        }
        if (s[i] == 'O') {
            i++;
        } else {
            assert(i + 2 <= len && s.substr(i, 2) == "II");
            ans += dp[len - i - 1];
            i += 2;
        }
    }
    return ans;
}