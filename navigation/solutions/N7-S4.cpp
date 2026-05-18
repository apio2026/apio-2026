#include "navigation.h"

#include <algorithm>
#include <numeric>
#include <iostream>
#include <vector>

std::vector<int> construct_network(int N, int K, int B, 
                                   std::vector<int> U, std::vector<int> V,
                                   std::vector<int> P)
{
    int root = 0;
    while (root < K && P[root] == root)
        root++;

    auto dfs = [&](auto &self, int u, int p) -> int
    {
        int cnt = 1;
        for (int i = 0; i < N - 1; i++)
        {
            if (U[i] == u && V[i] != p)
                cnt += self(self, V[i], u);
            if (V[i] == u && U[i] != p)
                cnt += self(self, U[i], u);
        }
        return std::min(cnt, K / 2);
    };

    std::vector T(N - 1, 0);
    for (int i = 0; i < N - 1; i++)
        if (U[i] == root)
            T[i] = dfs(dfs, V[i], root);
        else if (V[i] == root)
            T[i] = dfs(dfs, U[i], root);

    return T;
}

std::vector<int> navigate(int K, int B, std::vector<int> C)
{
    while (std::accumulate(C.begin(), C.end(), 0) < K)
        (*std::max_element(C.begin(), C.end()))++;
    return C;
}
