#include "nightmarket.h"

std::vector<std::string> find_sequences(int N, int M, std::vector<std::vector<int>> S)
{
    return std::vector(2, std::string(N - 1, 'D') + std::string(M - 1, 'R'));
}
