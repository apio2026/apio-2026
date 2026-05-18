#include "party.h"

using namespace std;

namespace {
    int N, K, p;
    int pred;
    vector<int> cnt;
}

void init(int _N, int _K, int _p, int _r) {
    N = _N;
    K = _K;
    p = _p;
    pred = 0;
    cnt.assign(N, 0);
}

int strategy(int b, int f, int s) {
    cnt[f]++;
    if (s < K)
        pred = max(pred, cnt[f]);
    if (p != f && cnt[f] == pred + 1)
        return N - p;
    else
        return 0;
}

vector<int> guess(int N, int K, vector<int> F, vector<int> S) {
    vector<int> ans(N), cnt(N, 0), vis(N, 0), last(N);
    for (int i = 0; i < N * N; i++) {
        last[vis[F[i]]] = i;
        vis[F[i]]++;
    }
    for (int i = 0; i < N; i++) {
        if (S[last[i]] == K)
            ans[i] = F[last[i]];
        else
            ans[i] = S[last[i]];
    }
    return ans;
}