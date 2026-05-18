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

    vector<int> X(cost + 1);
    vector<long long> Y(cost + 1);

    return {X, Y};
}
