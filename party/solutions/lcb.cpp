#include "party.h"

#include <algorithm>
#include <vector>

namespace
{
    // store global variables here
    std::vector<int> cnt;
    int i = 0;
    int P;
} // namespace

void init(int N, int K, int p, int r)
{
    cnt.resize(N, 0);
    P = p;
    return;
}

int strategy(int b, int f, int s)
{
    if (s == 0)
        i = std::max(i, cnt[f] + 1);
    if (cnt[f]++ == i && f != P)
        return s;
    return 0;
}

std::vector<int> guess(int N, int K, std::vector<int> F, std::vector<int> S)
{
    std::vector<int> P(N, -1);
    std::vector<int> used(N, 0);

    for (int i = 0; i < N; i++)
    {
        std::vector<int> vis(N, 0);
        for (int j = 0; j < N * N; j++)
            if (F[j] != -1 && !vis[F[j]] && !used[F[j]])
            {
                vis[F[j]] = 1;
                if (P[i] == -1) P[i] = F[j];
                if (S[j] == K)  P[i] = F[j];
                F[j] = -1;
            }
        used[P[i]] = 1;
    }

    return P;
}

