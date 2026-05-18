#include "apio.h"
#include <bits/stdc++.h>
using namespace std;

bitset<(1<<22)> vis;
void mark(int i, int d, int source, int n, vector<int> &decode) {
    decode[i] = source;
    vis[i] = 1;
    if (d >= 3) return;
    for (int j = 0;j < n-1;j++) {
        if (((i>>j)&1) == 0 && ((i>>(j+1))&1) == 1) {
            if (!vis[i - (1<<j)]) mark(i - (1<<j), d+1, source, n, decode);
        }
    }
}
bool build_set(int i, int d, int n) {
    if (vis[i]) return 0;
    if (d >= 3) return 1;
    for (int j = 0;j < n-1;j++) {
        if (((i>>j)&1) == 0 && ((i>>(j+1))&1) == 1) {
            bool val = build_set(i - (1<<j), d+1, n);
            if (val == 0) return 0;
        }
    }
    return 1;
}
void build_mapping(int n, vector<int> &encoded, vector<int> &decode) {
    decode.resize(1<<n);
    int cnt = 0;
    for (int i = (1<<n)-1;i >= 0;i--) {
        if (vis[i]) continue;
        if (build_set(i, 0, n)) {
            mark(i, 0, cnt, n, decode);
            encoded.push_back(i); 
            cnt++;
        }
    }
}

bool has_init = false;
vector<int> ea, eb;
vector<int> da, db;
int cnt45, cnt46, cnt47;
void init() {
    has_init = true;
    ea.clear();
    eb.clear();
    da.clear();
    db.clear();
    build_mapping(21, ea, da);
    vis.reset();
    build_mapping(22, eb, db);
    cnt45 = ea.size() * ea.size();
    cnt46 = ea.size() * eb.size();
    cnt47 = eb.size() * eb.size();
}

string ch = "IO";
string Alice(int x, int k){
    if (!has_init) {
        init();
    }
    string ret;
    if (x < cnt45) {
        int a = ea[x / ea.size()], b = ea[x % ea.size()];
        for (int i = 20;i >= 0;i--) ret += ch[(a>>i)&1];
        ret += "OOO";
        for (int i = 20;i >= 0;i--) ret += ch[(b>>i)&1];
    } else {
        x -= cnt45;
        if (x < cnt46) {
            int a = ea[x / eb.size()], b = eb[x % eb.size()];
            for (int i = 20;i >= 0;i--) ret += ch[(a>>i)&1];
            ret += "OOO";
            for (int i = 21;i >= 0;i--) ret += ch[(b>>i)&1];
        } else {
            x -= cnt46;
            int a = eb[x / eb.size()], b = eb[x % eb.size()];
            for (int i = 21;i >= 0;i--) ret += ch[(a>>i)&1];
            ret += "OOO";
            for (int i = 21;i >= 0;i--) ret += ch[(b>>i)&1];
        }
    }
    return ret;
}
int read_message(string s, int pad) {
    int ret = 0;
    int dig = s.size() - pad - 1; 
    for (char c:s) {
        if (c == 'O') {
            if (pad > 0) pad--;
            else {
                ret += (1<<dig); 
                dig--;
            }
        } else {
            dig--;
        }
    }
    return ret;
}
int Bob(string s, int k) {
    if (!has_init) {
        init();
    }
    int ret = 0;
    if (s.size() == 45) {
        int a = read_message(s.substr(0, 21), 0), b = read_message(s.substr(21, 24), 3);
        ret += da[a] * ea.size() + da[b];
    } else if (s.size() == 46) {
        ret += cnt45;
        int a = read_message(s.substr(0, 21), 0), b = read_message(s.substr(21, 25), 3);
        ret += da[a] * eb.size() + db[b];
    } else {
        ret = cnt45 + cnt46;
        int a = read_message(s.substr(0, 22), 0), b = read_message(s.substr(22, 25), 3);
        ret += db[a] * eb.size() + db[b];
    }
    return ret;
}
