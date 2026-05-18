// modified from general-S12, used to test if call constraints is held.
#include "navigation.h"

#include <algorithm>
#include <cstdlib>
#include <vector>
#include <utility>
#include <set>

int total_construct = 0;
int total_construct_sum_N = 0;
bool called_construct = false;

int total_navigate = 0;
int total_navigate_sum_C = 0;
bool called_navigate = false;

std::vector<int> construct_network(int N, int K, int B, 
                                   std::vector<int> U, std::vector<int> V,
                                   std::vector<int> P)
{
    called_construct = true;
    if (++total_construct > 10'000) std::abort();
    if ((total_construct_sum_N += N) > 100'000) std::abort();
    if (called_navigate) std::abort();
    if (K != 6) std::abort();
    


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
                color[i] += (K / 2 + 1) * (d % 3);
                self(self, v, u, d + 1);
            }
    };
    orient(orient, centroid, -1, 0);

    return color;
}

std::vector<int> navigate(int K, int B_arg, std::vector<int> C)
{
    called_navigate = true;
    if (++total_navigate > 100'000) std::abort();
    if ((total_navigate_sum_C += (int)C.size()) > 200'000) std::abort();
    if ((int)C.size() < B_arg) std::abort();
    if (called_construct) std::abort();


    int L = (int)C.size();
    int B = (K / 2 + 1);

    std::vector<int> D(L, 0);
    std::set<int> dist;

    for (int i = 0; i < L; i++)
    {
        D[i] = C[i] % B;
        dist.insert(C[i] / B);
    }

    int root_color = -1;
    if (dist == std::set<int>{0, 1})
        root_color = 0;
    if (dist == std::set<int>{1, 2})
        root_color = 1;
    if (dist == std::set<int>{2, 0})
        root_color = 2;

    for (int i = 0; i < L; i++)
        if (C[i] / B == root_color)
            D[i] = K - D[i];

    return D;
}
