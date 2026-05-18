#include "nightmarket.h"

std::vector<std::string> find_sequences(int N, int M, std::vector<std::vector<int>> S)
{
    for (int i = 0; i + 1 < N; i++)
        for (int j = 1; j < M; j++)
            if (S[i][j] == S[i + 1][j - 1])
                return std::vector(2, std::string(N - 1, 'S') + std::string(M - 1, 'E'));
    return {};
}
