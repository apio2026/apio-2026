#include "navigation.h"

#include <algorithm>
#include <array>
#include <cassert>
#include <numeric>
#include <utility>
#include <vector>


namespace
{
    enum cases
    {
        C3 = 0,
        C42 = 1,
        C411 = 2,
        C51 = 3,
        C6 = 4
    };

#define _ -1
    // clang-format off
    constexpr int encoding[2][5][5][4] = {
        {   // B >= 4
            // normal
            {{0, 1, 2, 3}, {_, _, _, _}, {_, _, _, _}, {_, _, _, _}, {_, _, _, _}},
            // 4-2
            {{3, _, 2, _}, {3, _, 0, _}, {3, _, 1, _}, {_, _, _, _}, {_, _, _, _}},
            // 4-1-1
            {{3, 1, _, _}, {3, 0, _, _}, {3, 0, _, _}, {_, _, _, _}, {_, _, _, _}},
            // 5-1
            {{2, 3, _, _}, {2, 0, _, _}, {_, _, _, _}, {2, 1, _, _}, {_, _, _, _}},
            // 6-0
            {{3, _, _, _}, {3, _, _, _}, {3, _, _, _}, {2, _, _, _}, {_, _, _, _}}
        },
        {   // B <= 3
            // normal
            {{0, 1, 2, 3}, {_, _, _, _}, {_, _, _, _}, {_, _, _, _}, {_, _, _, _}},
            // 4-2
            {{4, _, 2, _}, {4, _, 0, _}, {0, _, 1, _}, {_, _, _, _}, {_, _, _, _}},
            // 4-1-1
            {{4, 1, _, _}, {4, 3, _, _}, {4, 1, _, _}, {_, _, _, _}, {_, _, _, _}},
            // 5-1
            {{_, _, _, _}, {0, 3, _, _}, {_, _, _, _}, {2, 4, _, _}, {2, 1, _, _}},
            // 6-0
            {{4, _, _, _}, {_, _, _, _}, {3, _, _, _}, {0, _, _, _}, {2, _, _, _}}
        },
    };
#undef _
    // clang-format on
}

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

        int pattern = C3;
        if (children == 2 && max_children == 2)
            pattern = C42;
        if (children == 2 && max_children == 1)
            pattern = C411;
        if (children == 1)
            pattern = C51;
        if (children == 0)
            pattern = C6;

        while (encoding[B <= 3][pattern][prev][0] == -1) // must be special node
            prev = (prev + 1) % 5;

        for (auto [v, i] : adj[u])
        {
            color[i] = encoding[B <= 3][pattern][prev][subtree[v]];
            self(self, v, color[i]);
        }
    };
    orient(orient, centroid, 0);

    return color;
}

std::vector<int> navigate(int K, int B, std::vector<int> C)
{
    if (std::accumulate(C.begin(), C.end(), 0) == 6 &&
        *std::max_element(C.begin(), C.end()) <= 3)
        return C;

    int cnt[6] = {};
    for (auto c : C)
        cnt[c]++;

    auto validate = [&](int root_color, int root_size, const int color_map[4], std::array<int, 2> expected) -> std::vector<int>
    {
        if (cnt[root_color] != 1)
            return {};

        int cnt1 = color_map[1] == -1 ? 0 : cnt[color_map[1]];
        int cnt2 = color_map[2] == -1 ? 0 : cnt[color_map[2]];
        if (expected != std::array{cnt1, cnt2})
            return {};

        int reverse_map[5] = {-1, -1, -1, -1, -1};
        for (int i = 0; i < 4; i++)
            if (color_map[i] != -1)
                reverse_map[color_map[i]] = i;
        reverse_map[root_color] = root_size;

        for (int c = 0; c < 5; c++)
            if (cnt[c] && reverse_map[c] == -1)
                return {};

        auto D = C;
        for (auto &d : D)
            d = reverse_map[d];
        return D;
    };

    for (int i = 0; i < 5; i++)
    {
        if (auto D = validate(i, 4, encoding[B <= 3][C42][i], {0, 1}); !D.empty())
            return D;
        if (auto D = validate(i, 4, encoding[B <= 3][C411][i], {2, 0}); !D.empty())
            return D;
        if (auto D = validate(i, 5, encoding[B <= 3][C51][i], {1, 0}); !D.empty())
            return D;
        if (auto D = validate(i, 6, encoding[B <= 3][C6][i], {0, 0}); !D.empty())
            return D;
    }
    assert(false);
}
