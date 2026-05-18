// this is not a "real" solution, it is general-S5.cpp with 4 -> 8 to test scoring
#include "navigation.h"

#include <algorithm>
#include <cassert>
#include <map>
#include <numeric>
#include <set>
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

        int children = subtree[u] - mark[u];
        int max_children = 0;
        for (auto [v, _] : adj[u])
            max_children = std::max(max_children, subtree[v]);

        int palette[4] = {0, 1, 2, 3};

        if (children == 2 && max_children == 2) // 4-2
        {
            if      (prev == 1)     palette[2] = 0, palette[0] = 4;
            else if (prev == 0)     palette[2] = 2, palette[0] = 4;
            else  /* prev == 2 */   palette[2] = 1, palette[0] = 0;
        }
        else if (children == 2 && max_children == 1) // 4-1-1
        {
            palette[0] = 4;
            if     (prev == 1)                  palette[1] = 3;
            else /* prev == 2 || prev == 0 */   palette[1] = 1;
        }
        else if (children == 1) // 5-1
        {
            if      (prev == 1)     palette[1] = 3, palette[0] = 0;
            else if (prev == 3)     palette[1] = 4, palette[0] = 2;
            else  /* prev == 4 */   palette[1] = 1, palette[0] = 2;
        }
        else if (children == 0) // 6-0
        {
            if      (prev == 0)     palette[0] = 4;
            else if (prev == 4)     palette[0] = 2;
            else if (prev == 2)     palette[0] = 3;
            else  /* prev == 3 */   palette[0] = 0;
        }
        for (auto [v, i] : adj[u])
        {
            color[i] = palette[subtree[v]];
            self(self, v, color[i]);
        }
    };
    orient(orient, centroid, -1);

    // tweak 4 -> 8
    for (auto &c : color)
        if (c == 4) c = 8;

    return color;
}

std::vector<int> navigate(int K, int B, std::vector<int> C)
{
    // tweak 8 -> 4
    for (auto &c : C)
        if (c == 8) c = 4;

    if (std::accumulate(C.begin(), C.end(), 0) == 6 &&
        *std::max_element(C.begin(), C.end()) <= 3)
        return C;

    std::set<int> uniq(C.begin(), C.end());
    std::map<int, int> mp;
    for (auto c : C)
        mp[c]++;

    int ans[6] = {-1, -1, -1, -1, -1}; // one more slot for convenience

    auto check_singly_branch = [&](int root, int child, int zero, int answer)
    {
        if ((uniq == std::set({root, child}) || uniq == std::set({root, child, zero})) &&
            mp[root] == 1 && mp[child] == 1)
            ans[root] = answer, ans[child] = K - answer, ans[zero] = 0;
    };
    auto check_doubly_branch = [&](int root, int child, int zero, int answer)
    {
        if ((uniq == std::set({root, child}) || uniq == std::set({root, child, zero})) &&
            mp[root] == 1 && mp[child] == 2)
            ans[root] = answer, ans[child] = (K - answer) / 2, ans[zero] = 0;
    };
    auto check_6_0_branch = [&](int root, int zero)
    {
        if (uniq == std::set({root, zero}) && mp[root] == 1)
            ans[root] = K, ans[zero] = 0;
    };

    // 4-2
    check_singly_branch(1, 0, 4, 4);
    check_singly_branch(0, 2, 4, 4);
    check_singly_branch(2, 1, 0, 4);

    // 4-1-1
    check_doubly_branch(1, 3, 4, 4);
    check_doubly_branch(2, 1, 4, 4);
    check_doubly_branch(0, 1, 4, 4);

    // 5-1
    check_singly_branch(1, 3, 0, 5);
    check_singly_branch(3, 4, 2, 5);
    check_singly_branch(4, 1, 2, 5);

    // 6-0
    check_6_0_branch(0, 4);
    check_6_0_branch(4, 2);
    check_6_0_branch(2, 3);
    check_6_0_branch(3, 0);

    for (auto &c : C)
        c = ans[c];
    return C;
}
