#include "party.h"

using namespace std;

namespace {
    int N, K, p, r;
    vector<int> cnt;
}

void init(int _N, int _K, int _p, int _r) {
    N = _N;
    K = _K;
    p = _p;
    r = _r;
    cnt.assign(N, 0);
}

int strategy(int b, int f, int s) {
    cnt[f]++;
    if (cnt[f] - 1 == r) {
        if (p == f)
            return 0;
        else
            return s;
    } else {
        return 0;
    }
}

vector<int> guess(int N, int K, vector<int> F, vector<int> S) {
    vector<int> ans(N), cnt(N, 0), vis(N, 0);
    for (int i = 0; i < N * N; i++) {
        vis[F[i]]++;
        if (S[i] > 0)
            ans[vis[F[i]] - 1] = F[i];
    }
    return ans;
}