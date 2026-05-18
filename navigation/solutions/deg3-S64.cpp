#include "navigation.h"

#include <algorithm>
#include <vector>
#include <utility>

#include <iostream>

std::vector<int> construct_network(int N, int K, int B, 
                                   std::vector<int> U, std::vector<int> V,
                                   std::vector<int> P)
{
    std::vector<int> color(N - 1, 0);

    std::vector<std::vector<std::pair<int, int>>> adj(N);
    for (int i = 0; i < N - 1; i++)
    {
        adj[U[i]].emplace_back(V[i], i);
        adj[V[i]].emplace_back(U[i], i);
    }

    auto dfs = [&](auto self, int u, int p, int b) -> void
    {
        for (auto [v, i] : adj[u])
            if (v != p)
            {
                color[i] |= b;
                self(self, v, u, !b);
            }
    };
    for (int i = 0; i < K; i++)
    {
        for (auto &c : color)
            c <<= 1;
        dfs(dfs, P[i], -1, 0);
    }

    return color;
}

std::vector<int> navigate(int K, int B, std::vector<int> C)
{
    int L = (int)C.size();

    std::vector<int> D(L, 0);
    std::vector<int> bit_count(K, 0);
    for (int i = 0; i < L; i++)
        for (int j = 0; j < K; j++)
            bit_count[j] += (C[i] >> j & 1) ? +1 : -1;
    for (int i = 0; i < L; i++)
        for (int j = 0; j < K; j++)
            if ((C[i] >> j & 1) ^ (bit_count[j] > 0))
                D[i]++;
    return D;
}
