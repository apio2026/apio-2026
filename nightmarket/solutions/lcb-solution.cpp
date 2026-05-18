#include "nightmarket.h"

std::vector<std::string> find_sequences(int N, int M, std::vector<std::vector<int>> S)
{
    for (int i = 0; i + 1 < N; i++)
        for (int j = 1; j < M; j++)
            if (S[i][j] == S[i + 1][j - 1])
                return {std::string(i, 'S') + std::string(j - 1, 'E') + "ES" + std::string(N - i - 2, 'S') + std::string(M - j - 1, 'E'),
                        std::string(i, 'S') + std::string(j - 1, 'E') + "SE" + std::string(N - i - 2, 'S') + std::string(M - j - 1, 'E')};
    return {};
}
