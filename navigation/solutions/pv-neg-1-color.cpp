#include "navigation.h"

#include <cstdlib>

std::vector<int> construct_network(int N, int K, int B, 
                                   std::vector<int> U, std::vector<int> V,
                                   std::vector<int> P)
{
    return std::vector(N - 1, -1);
}

std::vector<int> navigate(int K, int _, std::vector<int> C)
{
    // never called because network always invalid
    std::abort();
    return {};
}
