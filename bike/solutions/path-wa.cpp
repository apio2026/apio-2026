#include "bike.h"

#include <bits/stdc++.h>
using namespace std;

std::pair<std::vector<int>, std::vector<long long>>
        find_rebalancing_strategy(int N, 
                                  std::vector<int> A, 
                                  std::vector<int> B, 
                                  std::vector<int> U,
                                  std::vector<int> V) {
    
    vector<long long> sum(N);
    for (int i = 0; i < N; i++) {
        sum[i] = A[i] - B[i];
        if (i) sum[i] += sum[i - 1];
    }

    int st = 0, ed = 0, dir = 1, cost = 2 * N - 2;
    
    { // go left
        int cur = 0;
        int mx = 0;
        int endpoint = 0;
        for (int i = 0; i < N - 1; i++) {
            if (sum[i] <= 0) cur = max(cur + 1, 0);
            else cur = max(cur - 1, 0);
            if (cur == 0) endpoint = i + 1;
            mx = max(mx, cur);
            int tmp = 2 * (N - 1) - cur;
            if (tmp < cost) 
                st = i + 1, ed = endpoint, dir = -1, cost = tmp;
        }
    }
    { // go right
        int cur = 0;
        int mx = 0;
        int endpoint = N - 1;
        for (int i = N - 2; i >= 0; i--) {
            if (sum[i] >= 0) cur = max(cur + 1, 0);
            else cur = max(cur - 1, 0);
            if (cur == 0) endpoint = i;
            mx = max(mx, cur);
            int tmp = 2 * (N - 1) - cur;
            if (tmp < cost) 
                st = i, ed = endpoint, dir = 1, cost = tmp;
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

    if (dir == 1) {
        for (int i = st; i >= 0; i--) walk(i, -A[i]);
        for (int i = 0; i <= st; i++) walk(i, B[i]);
        for (int i = st + 1; i < ed; i++) {
            vector<int> todo;
            while (i + 1 < ed && sum[i] < 0) {
                todo.emplace_back(i);
                i++;
            }
            todo.emplace_back(i);
            for (int j : todo) walk(j, -A[j]);
            for (int j : todo | views::reverse) walk(j, -A[j]);
            for (int j : todo) walk(j, B[j]);
        }
        for (int i = ed; i < N; i++) walk(i, -A[i]);
        for (int i = N - 1; i >= ed; i--) walk(i, B[i]);
    }
    else {
        for (int i = st; i < N; i++) walk(i, -A[i]);
        for (int i = N - 1; i >= st; i--) walk(i, B[i]);
        for (int i = st - 1; i >= ed; i--) {
            vector<int> todo;
            while (i - 1 >= 0 && sum[i] > 0) {
                todo.emplace_back(i);
                i--;
            }
            todo.emplace_back(i);
            for (int j : todo) walk(j, -A[j]);
            for (int j : todo | views::reverse) walk(j, -A[j]);
            for (int j : todo) walk(j, B[j]);
        }
        for (int i = ed; i >= 0; i--) walk(i, -A[i]);
        for (int i = 0; i <= ed; i++) walk(i, B[i]);
    }
    assert(ssize(X) - 1 == cost);

    return {X, Y};
}
