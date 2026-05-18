#include "navigation.h"

#include <algorithm>
#include <vector>
#include <map>
#include <utility>

#include <iostream>

std::vector<int> construct_network(int N, int K, int B,
                                   std::vector<int> U, std::vector<int> V,
                                   std::vector<int> P)
{
    std::vector<int> color(N - 1, -1);
    auto fill = [&](int start, int end, std::vector<int> pattern)
    {
        for (int i = start; i < end; i++)
            color[i] = pattern[i % pattern.size()];
    };

    fill(0, P[0], {0, 1, 2});
    fill(P[0], P[1], {0, 3, 4});
    fill(P[1], P[2], {1, 4, 2, 3});
    fill(P[2], P[3], {0, 0});
    fill(P[3], P[4], {3, 2, 4, 1});
    fill(P[4], P[5], {4, 3, 0});
    fill(P[5], N - 1, {2, 1, 0});

    return color;
}

std::vector<int> navigate(int K, int B,std::vector<int> C)
{
    auto resolve = [](std::vector<int> C) -> std::vector<int>
    {
        if (C == std::vector{0, 1} || C == std::vector{1, 2} || C == std::vector{2, 0})
            return {0, 6};
        if (C == std::vector{0, 3} || C == std::vector{3, 4} || C == std::vector{4, 0})
            return {1, 5};
        if (C == std::vector{1, 4} || C == std::vector{4, 2} || C == std::vector{2, 3} || C == std::vector{3, 1})
            return {2, 4};
        if (C == std::vector{0, 0})
            return {3, 3};
        return {};
    };

    if (C.size() == 2)
    {
        if (auto r = resolve(C); !r.empty())
            return r;

        std::reverse(C.begin(), C.end());

        if (auto r = resolve(C); !r.empty())
        {
            std::reverse(r.begin(), r.end());
            return r;
        }
    }
    return {};
}
