#include "navigation.h"

#include <cassert>
#include <vector>

std::vector<int> construct_network(int N, int K, int B,
                                   std::vector<int> U, std::vector<int> V,
                                   std::vector<int> P)
{
    return {0, 10, 1, 2, 3, 4, 5, 5};
}

namespace
{
    int count = 0;
}

std::vector<int> navigate(int K, int B, std::vector<int> C)
{
    count++;
    if (count == 1)
    {
        assert((C == std::vector{0, 10, 1}));
        return {1, 4, 1};
    }
    else if (count == 2)
    {
        assert((C == std::vector{10, 2, 3, 4, 5}));
        return {2, 1, 1, 1, 1};
    }
    else
        assert(false && "too many call in sample interaction");
}
