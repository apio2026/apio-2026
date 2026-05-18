#include "navigation.h"

#include <thread>
#include <chrono>

using namespace std::chrono_literals;


std::vector<int> construct_network(int N, int K, int B, 
                                   std::vector<int> U, std::vector<int> V,
                                   std::vector<int> P)
{
    std::this_thread::sleep_for(10s);
    return std::vector(N - 1, 0);
}

std::vector<int> navigate(int K, int _, std::vector<int> C)
{
    return C;
}
