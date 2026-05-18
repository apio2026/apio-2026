#include "apio.h"
#include <bits/stdc++.h>
using namespace std;

string Alice(int x, int k){
    string res;
    for (int i = 0; i < 30; ++i) {
        if (x >> i & 1) {
            res += "OO";
        } else {
            res += "II";
        }
    }
    return res;
}

int Bob(string s, int k) {
    int ans = 0;
    for (int i = 0; i < 30; ++i) {
        while (s[i * 2] != s[i * 2 + 1]) {
            int id = s[i * 2] == 'O' ? i * 2 + 1 : i * 2;
            int gid = find(s.begin() + id, s.end(), 'O') - s.begin();
            swap(s[id], s[gid]);
        }
        assert(s[i * 2] == s[i * 2 + 1]);
        if (s[i * 2] == 'O') {
            ans |= 1 << i;
        }
    }
    return ans;
}