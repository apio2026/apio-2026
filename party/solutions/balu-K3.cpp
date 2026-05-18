#include "party.h"
#include <iostream>
#include <algorithm>
#include <ranges>
#include <cassert>

namespace {
    int ban = 0, num = 0, eaten = 0, k = 0;
    std::vector<int> cnt;
}

void init(int N, int K, int p, int r) {
    cnt.resize(N);
    ban = p;
    k = K;
    return;
}

int strategy(int b, int f, int s) {
    int base = 0;
    if (s == 0 && num == cnt[f]) ++num;
    if (s > 0 && s == k - 1) {
        ++num;
        base = 1;
    }
    ++cnt[f];
    if (num == cnt[f] - 1 && s > 0) {
        if (f != ban) {
            eaten = 1;
            return s;
        }
        if (!eaten) eaten = -1;
    }
    if (eaten == -1 && f != ban && s > 0) {
        eaten = 1;
        return 1;
    }
    return base;
}

std::vector<int> guess(int N, int K, std::vector<int> F, std::vector<int> S) {
    std::vector<int> first(N, -1), assigned(N), cnt(N);
    std::vector<int> output(N, -1);
    for (int i = 0; i < N * N; ++i) {
        if (S[i] > 0) {
            if (output[cnt[F[i]]] != -1)
                assigned[output[cnt[F[i]]]] = 0;
            output[cnt[F[i]]] = F[i];
            first[cnt[F[i]]] = -2;
            assigned[F[i]] = 1;
        }
        ++cnt[F[i]];
    }
    std::ranges::fill(cnt, 0);
    for (int i = 0; i < N * N; ++i) {
        if (first[cnt[F[i]]] == -1 && !assigned[F[i]]) {
            first[cnt[F[i]]] = F[i];
            assigned[F[i]] = 1;
        }
        ++cnt[F[i]];
    }
    for (int i = 0; i < N; ++i)
        if (output[i] == -1)
            output[i] = first[i];
    return output;
}
