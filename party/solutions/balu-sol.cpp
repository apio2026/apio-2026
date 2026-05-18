#include "party.h"

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
    std::vector<int> done(N), cnt(N), cntmap(N * N);
    std::vector<int> output(N, -1);
    for (int i = 0; i < N * N; ++i) {
        if (S[i] == K)
            output[cnt[F[i]]] = F[i];
        cntmap[i] = cnt[F[i]];
        ++cnt[F[i]];
    }
    for (int i = 0; i < N; ++i)
        if (output[i] != -1)
            done[output[i]] = 1;
    for (int i = 0; i < N * N; ++i) {
        if (output[cntmap[i]] == -1 && !done[F[i]]) {
            output[cntmap[i]] = F[i];
            done[F[i]] = 1;
        }
    }
    return output;
}
