#include "bike.h"

#include <bits/stdc++.h>
using namespace std;

std::pair<std::vector<int>, std::vector<long long>>
        find_rebalancing_strategy(int N, 
                                  std::vector<int> A, 
                                  std::vector<int> B, 
                                  std::vector<int> U,
                                  std::vector<int> V) {
    
    int st = -1;
    for (int i = 0; i < N; i++) {
        if (A[i]) {
            st = i;
            break;
        }
    }

    vector<int> X;
    vector<long long> Y;
    auto walk = [&](int v, long long amount) {
        if (X.empty() || X.back() != v) X.emplace_back(v), Y.emplace_back(0);
        Y.back() += amount;
        A[v] += amount;
        assert(A[v] >= 0);
    };
    
    int right = N - 1 - st + N - 1;
    int left = st + N - 1;
    if (right <= left) {
        for (int i = st; i < N; i++) walk(i, -A[i]);
        for (int i = N - 1; i >= 0; i--) walk(i, B[i]);
    }
    else {
        for (int i = st; i >= 0; i--) walk(i, -A[i]);
        for (int i = 0; i < N; i++) walk(i, B[i]);
    }
    
    return {X, Y};
}
