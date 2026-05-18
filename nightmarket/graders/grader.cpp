#include "nightmarket.h"
#include <cassert>
#include <cstdio>
// BEGIN SECRET
#include <algorithm>
// END SECRET

int main()
{
  // BEGIN SECRET
  const std::string input_secret = "ME7ejctSVkoKUc57oSEJdTsVO92dpT3k";
  const std::string output_secret = "oOHMFNtb7jCK0WnXKrePqIIUlLLAkfMo";
  char secret[256];
  assert(scanf("%255s", secret) == 1);
  if (std::string(secret) != input_secret)
  {
    printf("%s\n", output_secret.c_str());
    printf("PV\n");
    printf("Possible tampering with the input\n");
    fclose(stdout);
    return 0;
  }
  std::fill_n(secret, sizeof(secret), 0);
  // END SECRET
  int N, M;
  assert(scanf("%d", &N) == 1);
  assert(scanf("%d", &M) == 1);
  std::vector<std::vector<int>> S(N, std::vector<int>(M));
  for (int i = 0; i < N; i++)
    for (int j = 0; j < M; j++)
      assert(scanf("%d", &S[i][j]) == 1);
  fclose(stdin);

  std::vector<std::string> P = find_sequences(N, M, S);

  // BEGIN SECRET
  printf("%s\n", output_secret.c_str());
  printf("OK\n");
  // END SECRET
  printf("%d\n", (int)P.size());
  for (size_t i = 0; i < P.size(); i++)
    printf("%s\n", P[i].c_str());
  fclose(stdout);

  return 0;
}
