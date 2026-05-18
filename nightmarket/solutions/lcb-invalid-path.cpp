#include "nightmarket.h"

std::vector<std::string> find_sequences(int N, int M, std::vector<std::vector<int>> S)
{
    return {std::string(N + M - 2, 'S'),  std::string(N + M - 2, 'E')};
}
