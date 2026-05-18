#include "party.h"
#include <utility>

using namespace std;

namespace {
    int N, K, P, pred;
    vector<int> cnt;
}

void init(int _N, int _K, int p, int r) {
    N = _N;
    K = _K;
    P = p;
    pred = 0;
    cnt.assign(N, 0);
}

int strategy(int b, int f, int s) {
    cnt[f]++;
    if (s == 0)
        pred = max(pred, cnt[f]);

    if (P != f && cnt[f] == pred + 1)
        return s;
    else
        return 0;
}

vector<int> guess(int N, int K, vector<int> F, vector<int> S) {
    vector<int> ans(N, -1), cnt(N, 0);
    vector<bool> done(N, false);
    vector<vector<pair<int, int>>> g(N);
    for (int i = 0; i < N * N; i++) {
        cnt[F[i]]++;
        g[cnt[F[i]] - 1].push_back({F[i], S[i]});
    }
    for (int i = 0; i < N; i++) {
        for (auto [col, val] : g[i]) {
            if (!done[col] && val == K) {
                ans[i] = col;
                done[col] = true;
                break;
            }
        }
        if (ans[i] == -1) {
            for (auto [col, val] : g[i]) {
                if (!done[col]) {
                    ans[i] = col;
                    done[col] = true;
                    break;
                }
            }
        }
    }
    return ans;
}
