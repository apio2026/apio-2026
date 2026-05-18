#include <bits/stdc++.h>
using namespace std;
#include "ancient.h"
using ll = long long;

void print_array(const std::vector<int> &v) {
    std::cerr << "return:";
    for (int i : v) std::cerr << " " << i;
    std::cerr << "\n";
}

const ll maxc = 1'000'005;
vector<int> get_tree(ll N) {
    //first get degree of each node
    vector<ll> que(N);
    for (int i = 0;i < N;i++) que[i] = (N - i) * maxc;
    vector<int> child_cnt = detect_convenience_values(que); 
    vector<int> p(N);    
    for (int b = 16;b >= 0;b--) {
        int zeros = 0, ones = 0; 
        for (int i = 0;i < N;i++) {
            if ((i >> b) & 1) {
                que[i] = (ones - maxc) * maxc;
                ones++;
            } else {
                que[i] = (maxc - zeros) * maxc;
                zeros++;
            }
        }
        vector<int> res = detect_convenience_values(que);
        for (int i = 0;i < N;i++) {
            if ((i >> b) & 1) {
                if (res[i] == 1) p[i] |= 1<<b;
            } else {
                if (res[i] == child_cnt[i] + 1) p[i] |= 1<<b;
            }
        }
    }
    return p; 
}

vector<int> find_heights(vector<int> p, int C) {
    int N = p.size();
    vector<int> l(N, -C), r(N, C), m(N);
    vector<ll> que(N, 0);
    vector<int> heights(N);
    for (int t = 0;t < 21;t++) {
        for (int i = 1;i < N;i++) {
            m[i] = (l[i] + r[i]) / 2;
            que[i] = que[p[i]] + m[i];
        }
        vector<int> res = detect_convenience_values(que);
        for (int i = N-1;i >= 0;i--) {
            if (res[i] == 1) {
                r[i] = m[i];
            } else {
                l[i] = m[i];
                res[p[i]]--;
            }
        }
        for (int i = 1;i < N;i++) {
            heights[i] = heights[p[i]] - r[i];
        }
    }
    return heights;
}
std::pair<std::vector<int>, std::vector<int>> find_information(int N, int C) {
    std::vector<int> p = get_tree(N);
    vector<int> heights = find_heights(p, C);
    p.erase(p.begin());
    heights.erase(heights.begin());
    //print_array(p);
    //print_array(heights);
    return std::make_pair(p, heights);
}
