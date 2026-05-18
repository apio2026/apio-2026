#include "navigation.h"

#include <algorithm>
#include <cassert>
#include <map>
#include <numeric>
#include <utility>
#include <vector>

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

    std::vector<int> mark(N, 0);
    for (int i = 0; i < K; i++)
        mark[P[i]] = 1;

    std::vector<int> subtree = mark;
    std::vector<int> parent(N, -1);

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

    int centroid = -1;
    for (int u = 0; u < N; u++)
    {
        bool is_centroid = (K - subtree[u]) <= K / 2;
        for (auto [v, i] : adj[u])
            if (v != parent[u])
                is_centroid &= subtree[v] <= K / 2;

        if (is_centroid)
            centroid = u;
    }

    subtree = mark;
    std::fill(parent.begin(), parent.end(), -1);
    dfs(dfs, centroid);

    auto orient = [&](auto self, int u, int prev) -> void
    {
        for (auto iter = adj[u].begin(); iter != adj[u].end(); ++iter)
            if (iter->first == parent[u])
            {
                adj[u].erase(iter);
                break;
            }

        if (subtree[u] - mark[u] >= 3)
            for (auto [v, i] : adj[u])
            {
                color[i] = subtree[v];
                self(self, v, color[i]);
            }
        else if (subtree[u] - mark[u] == 2) // 4
            for (auto [v, i] : adj[u])
            {
                assert(prev == 0 || prev == 1 || prev == 2 || mark[u]);
                if (subtree[v] == 2) // 4-2
                    color[i] = (prev == 2) * 1 +
                               (prev == 1) * 0 +
                               (prev == 0) * 2;

                else if (subtree[v] == 1) // 4-1-1
                    color[i] = (prev == 2) * 0 +
                               (prev == 1) * 0 +
                               (prev == 0) * 1;

                else
                    color[i] = 3;

                self(self, v, color[i]);
            }

        else if (subtree[u] - mark[u] == 1) // 5
            for (auto [v, i] : adj[u])
            {
                if (subtree[v] == 1) // 5-1
                    color[i] = (prev == 0) * 3 +
                               (prev == 1) * 0 +
                               (prev == 3) * 1;

                else
                    color[i] = 2;

                self(self, v, color[i]);
            }
        else if (subtree[u] - mark[u] == 0)
            for (auto [v, i] : adj[u])
            {
                color[i] = (prev == 3 ? 2 : 3);
                self(self, v, color[i]);
            }
    };
    orient(orient, centroid, -1);

    return color;
}

std::vector<int> navigate(int K, int B, std::vector<int> C)
{
    if (std::accumulate(C.begin(), C.end(), 0) == 6)
        return C;

    std::map<int, int> mp;
    for (auto c : C)
        mp[c]++;

    if (mp.size() == 2) // 6-0
    {
        std::vector<int> D = C;
        for (auto &d : D)
            d = (mp[d] == 1) ? 6 : 0;
        return D;
    }
    if (mp[2] >= 2) // 5-1
    {
        int root_color = -1;
        if (mp[0] && mp[1])
            root_color = 1;
        if (mp[0] && mp[3])
            root_color = 0;
        if (mp[1] && mp[3])
            root_color = 3;

        std::vector<int> D = C;
        for (auto &d : D)
            d = (d == root_color) ? 5 : (d != 2 ? 1 : 0);
        return D;
    }

    // 4-2
    if (mp[3] + 2 == (int)C.size()) // 4-2
    {
        int root_color = -1;
        if (mp[2] && mp[1])
            root_color = 2;
        if (mp[1] && mp[0])
            root_color = 1;
        if (mp[0] && mp[2])
            root_color = 0;

        std::vector<int> D = C;
        for (auto &d : D)
            d = (d == root_color) ? 4 : (d != 3 ? 2 : 0);
        return D;
    }
    if (mp[3] + 3 == (int)C.size()) // 4-1-1
    {
        std::vector<int> D = C;
        for (auto &d : D)
            d = (d == 3) ? 0 : (mp[d] == 1 ? 4 : 1);
        return D;
    }
    assert(false);
    return {};
}
