#include "bike.h"

#include <bits/stdc++.h>
using namespace std;

std::pair<std::vector<int>, std::vector<long long>>
        find_rebalancing_strategy(int N, 
                                  std::vector<int> A, 
                                  std::vector<int> B, 
                                  std::vector<int> U,
                                  std::vector<int> V) {
    assert(N <= 7);

    vector<int> seq;

    vector<int> X(2 * N);
    vector<long long> Y(2 * N);

    auto check = [&]() {
        vector<long long> change(ssize(seq));
        vector<bool> vst(N);
        /*cerr << "test ";
        for (int i : seq) cerr << i << " ";
        cerr << "\n";*/
        int idx = 0;
        for (int i : seq) {
            if(!vst[i]) {
                vst[i] = true;
                change[idx] += -A[i];
            }
            idx++;
        }
        fill(vst.begin(), vst.end(), false);
        idx = ssize(seq) - 1;
        for (int i : seq | views::reverse) {
            if(!vst[i]) {
                vst[i] = true;
                change[idx] += B[i];
            }
            idx--;
        }
        /*cerr << "change ";
        for (int i : change) cerr << i << " ";
        cerr << "\n";*/
        for (int i = 0; i < N; i++)
            if (!vst[i]) return;
        long long sum = 0;
        for (long long i : change) {
            sum += i;
            if (sum > 0) return;
        }
        if (ssize(seq) < ssize(X)) {
            X = seq;
            Y = change;
        }
    };

    auto dfs1 = [&](auto self, int now, int len) -> void {
        if (len > 2 * N) return;
        seq.emplace_back(now);
        check();
        if (now) self(self, now - 1, len + 1);
        if (now + 1 < N) self(self, now + 1, len + 1);
        seq.pop_back();
    };
    for (int i = 0; i < N; i++) {
        dfs1(dfs1, i, 0);
    }

    return {X, Y};
}
