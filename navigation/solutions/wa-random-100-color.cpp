#include "navigation.h"

#include <limits>
#include <cstdlib>
#include <random>

std::mt19937 rd(0xf157);

std::vector<int> construct_network(int N, int K, int B, 
                                   std::vector<int> U, std::vector<int> V,
                                   std::vector<int> P)
{
    std::vector<int> T(N - 1);
    for (auto &t : T)
        t = rd() % 100;
    return T;
}

std::vector<int> navigate(int K, int _, std::vector<int> C)
{
    return {};
}
