#include "bike.h"
#include <cassert>
#include <cstdio>
// BEGIN SECRET
#include <algorithm>
#include <string>
namespace {
  const std::string input_secret = "mmZy4HsWSjUc48Lu9gyzHZGCR4L2y0gM";
  const std::string output_secret = "yCEtdR8YvssvtZayQMicBE5e4bPV1VJ3";
  void read_secret() {
    char secret[256];
    assert(scanf("%255s", secret) == 1);
    if (std::string(secret) != input_secret) {
      printf("%s\n", output_secret.c_str());
      printf("PV\n");
      printf("Possible tampering with the input\n");
      fclose(stdout);
      exit(0);
    }
    std::fill_n(secret, sizeof(secret), 0);
  }
  void write_secret() {
    printf("%s\n", output_secret.c_str());
  }
}
// END SECRET

int main() {

// BEGIN SECRET
  read_secret();
// END SECRET
  int T;
  assert(scanf("%d", &T) == 1);
  
  std::vector<int> Nt(T);
  std::vector<std::vector<int>> At(T);
  std::vector<std::vector<int>> Bt(T);
  std::vector<std::vector<int>> Ut(T);
  std::vector<std::vector<int>> Vt(T);

  for (int t = 0; t < T; ++t) {
    int N;
    assert(scanf("%d", &N) == 1);
    Nt[t] = N;
    std::vector<int> A(N), B(N), U(N - 1), V(N - 1);
    for (int i = 0; i < N; i++)
      assert(scanf("%d", &A[i]) == 1);
    for (int i = 0; i < N; i++)
      assert(scanf("%d", &B[i]) == 1);
    for (int i = 0; i < N - 1; i++)
      assert(scanf("%d%d", &U[i], &V[i]) == 2);
    At[t] = A;
    Bt[t] = B;
    Ut[t] = U;
    Vt[t] = V;
  }
  fclose(stdin);
  
  std::vector<std::pair<std::vector<int>, std::vector<long long>>> ans(T);
  for (int t = 0; t < T; ++t) {
    auto ret = find_rebalancing_strategy(Nt[t], At[t], Bt[t], Ut[t], Vt[t]);
    // BEGIN SECRET
    if (ret.first.size() != ret.second.size() || int(ret.first.size()) > 2 * Nt[t] - 1) {
        write_secret();
        printf("WA\n");
        return 0;
    }
    // END SECRET
    ans[t] = ret;
  }
  
  // BEGIN SECRET
  write_secret();
  printf("OK\n");
  // END SECRET
  for (int t = 0; t < T; ++t) {
    auto [X, Y] = ans[t];
    int k = int(X.size()) - 1;
    printf("%d\n", k);
    for (int j = 0; j <= k; j++)
      printf("%d%c", X[j], " \n"[j == k]);
    for (int j = 0; j <= k; j++)
      printf("%lld%c", Y[j], " \n"[j == k]);
  }
  fclose(stdout);
  return 0;
}
