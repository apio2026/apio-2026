#include "party.h"

using namespace std;

namespace {
    int N, K, p, cnt, cur;
}

void init(int _N, int _K, int _p, int r) {
    N = _N;
    K = _K;
    p = _p;
    cnt = 0;
    cur = 0;
}

int strategy(int b, int f, int s) {
    if (b < N) {
        if (s == 0) cnt++;
        if (f == (p + 1) % N)
            return s;
        else
            return 0;
    } else {
        if (f != (p + 1) % N)
            return 0;
        else {
            cur++;
            if (cur == cnt)
                return 0;
            else
                return s;
        }
    }
}

vector<int> guess(int N, int K, vector<int> F, vector<int> S) {
    vector<int> inv(N, 0), vis(N, 0);
    for (int i = N; i < N * N; i++) {
        vis[F[i]]++;
        if (S[i] > 0) inv[(F[i] + N - 1) % N] = vis[F[i]];
    }
    vector<int> ans(N);
    for (int i = 0; i < N; i++)
        ans[inv[i]] = i;
    return ans;
}