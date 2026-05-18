#include "nightmarket.h"
#include <valarray>

std::vector<std::string> find_sequences(int N, int M, std::vector<std::vector<int>> S)
{
    for (int i = 0; i + 1 < N; i++)
    {
        for (int j = 0; j + 1 < M; j++)
        {
            if (S[i + 1][j] == S[i][j + 1])
            {
                std::string A = "", B = "";
                for (int k = 0; k < i; k++)
                {
                    A += "S";
                    B += "S";
                }
                for (int k = 0; k < j; k++)
                {
                    A += "E";
                    B += "E";
                }
                A += "SE";
                B += "ES";
                for (int k = 0; k < N - i - 2; k++)
                {
                    A += "S";
                    B += "S";
                }
                for (int k = 0; k < M - j - 2; k++)
                {
                    A += "E";
                    B += "E";
                }
                return {A, A};
            }
        }
    }
    return {};
}
