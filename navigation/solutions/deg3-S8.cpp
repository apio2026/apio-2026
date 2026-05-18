#include "navigation.h"

#include <algorithm>
#include <vector>
#include <utility>
#include <set>

/*
S = 12 = 4 * 3
use lower 4 to store {0, 6}, {1, 5}, {2, 4}, {3},
and upper 2 to store distance to centroid mod 2
*/

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

    int centroid = -1;
    {
        std::vector<int> subtree(N, 0);
        std::vector<int> parent(N, -1);
        for (int i = 0; i < K; i++)
            subtree[P[i]]++;

        std::vector<int> marked = subtree;

        auto dfs = [&](auto self, int u) -> void
        {
            for (auto [v, i] : adj[u])
                if (v != parent[u])
                {
                    parent[v] = u;
                    self(self, v);
                    subtree[u] += subtree[v];
                }
        };
        dfs(dfs, 0);

        for (int u = 0; u < N; u++)
        {
            bool is_centroid = true;
            for (auto [v, i] : adj[u])
            {
                int sub = (v == parent[u] ? K - subtree[u] : subtree[v]);
                color[i] = std::min(sub, K - sub);
                if (sub > K / 2)
                    is_centroid = false;
            }
            if (is_centroid)
                centroid = u;
        }
    }

    auto orient = [&](auto self, int u, int p, int d) -> void
    {
        for (auto [v, i] : adj[u])
            if (v != p)
            {
                color[i] += (K / 2 + 1) * (d % 2);
                self(self, v, u, d + 1);
            }
    };
    orient(orient, centroid, -1, 0);

    return color;
}

std::vector<int> navigate(int K, int _, std::vector<int> C)
{
    int L = (int)C.size();
    int B = (K / 2 + 1);

    std::vector<int> D(L, 0);
    std::vector<int> dist(2);

    for (int i = 0; i < L; i++)
    {
        D[i] = C[i] % B;
        dist[C[i] / B]++;
    }

    for (int i = 0; i < L; i++)
        if (dist[C[i] / B] == 1)
            D[i] = K - D[i];

    return D;
}
