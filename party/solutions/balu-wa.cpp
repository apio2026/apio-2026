#include "party.h"
#include <numeric>
#include <algorithm>

namespace {
    int ban = 0, num = 0;
    std::vector<int> cnt;
}

void init(int N, int K, int p, int r) {
    cnt.resize(N);
    ban = p;
    return;
}

int strategy(int b, int f, int s) {
    if (s == 0 && num == cnt[f]) ++num;
    ++cnt[f];
    if (num == cnt[f] - 1 && s > 0 && f != ban)
        return s;
    return 0;
}

std::vector<int> guess(int N, int K, std::vector<int> F, std::vector<int> S) {
    std::vector<int> first(N, -1), cnt(N);
    std::vector<int> output(N, -1);
    for (int i = 0; i < N * N; ++i) {
        if (first[cnt[F[i]]] == -1)
            first[cnt[F[i]]] = F[i];
        if (S[i] == K)
            output[cnt[F[i]]] = F[i];
        ++cnt[F[i]];
    }
    for (int i = 0; i < N; ++i)
        if (output[i] == -1)
            output[i] = first[i];
    return output;
}
