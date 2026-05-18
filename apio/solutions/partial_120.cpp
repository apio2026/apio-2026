#include "apio.h"
#include <bits/stdc++.h>
using namespace std;

string Alice(int x, int k) {
    string s;
    for (int i = 29; ~i; --i) {
        if (x >> i & 1) s += "OIII";
        else s += "IIII";
    }
    return s;
}

int Bob(string s, int k) {
    int ans = 0;
    for (int i = 0; i < (int)s.size(); i += 4) {
        ans *= 2;
        if (s[i] == 'O' || s[i + 1] == 'O' || s[i + 2] == 'O' || s[i + 3] == 'O') ans++;
    }
    return ans;
}