#include "testlib.h"

#include <string>
#include <algorithm>
#include <vector>

const std::string output_secret = "yCEtdR8YvssvtZayQMicBE5e4bPV1VJ3";

struct SolutionInformation {
    bool valid;
    int k;
};

int main(int argc, char *argv[]) {
    registerChecker("bike", argc, argv);
    readBothSecrets(output_secret);
    readBothGraderResults();
    inf.readToken(); // secret

    bool partial = false;

    int T = inf.readInt();
    while (T--) {
        
        int N = inf.readInt();

        std::vector<int> A(N), B(N);
        std::vector<std::pair<int, int>> edges;
        for (int i = 0; i < N; i++) A[i] = inf.readInt();
        for (int i = 0; i < N; i++) B[i] = inf.readInt();
        for (int i = 0; i < N - 1; i++) {
            int u = inf.readInt(), v = inf.readInt();
            if (u > v) std::swap(u, v);
            edges.emplace_back(std::make_pair(u, v));
        }
        std::sort(edges.begin(), edges.end());
        auto check_edge = [&](int u, int v) {
            if (u > v) std::swap(u, v);
            return std::ranges::binary_search(edges, std::make_pair(u, v));
        };

        auto read_solution = [&](InStream &in) {
            int k = in.readInt(0, 2 * N - 2);
            std::vector<int> X(k + 1);
            std::vector<long long> Y(k + 1);
            for (int i = 0; i <= k; i++) X[i] = in.readInt();
            for (int i = 0; i <= k; i++) Y[i] = in.readLong();
            const long long MAX_BIKES = 1'000'000LL * 1'000'000'000LL;
            bool valid = [&]() {
                for (int i = 0; i <= k; i++)
                    if (X[i] < 0 || N <= X[i]) return false;
                for (int i = 0; i <= k; i++)
                    if (std::abs(Y[i]) > MAX_BIKES) return false;
                for (int i = 0; i < k; i++) 
                    if (!check_edge(X[i], X[i + 1])) return false;
                long long sum = 0;
                for (int i = 0; i <= k; i++) {
                    sum += Y[i];
                    if (sum > 0 || std::abs(sum) > MAX_BIKES) return false;
                }
                std::vector<long long> tmp(A.begin(), A.end());
                for (int i = 0; i <= k; i++) {
                    tmp[X[i]] += Y[i];
                    if (tmp[X[i]] < 0) return false;
                }
                for (int i = 0; i < N; i++)
                    if (tmp[i] != B[i]) return false;

                return true;
            }();
            return SolutionInformation(valid, k);
        };

        auto sol_ouf = read_solution(ouf);
        auto sol_ans = read_solution(ans);

        if (!sol_ans.valid) quitf(_fail, "Jury's solution is invalid");

        if (sol_ouf.valid) {
            if (sol_ouf.k > sol_ans.k) quitf(_wa, "Not optimal solution");
            else if (sol_ouf.k < sol_ans.k) quitf(_fail, "Participant's solution is better (cost)");
            else continue;
        }
        else if (sol_ouf.k == sol_ans.k) partial = true;
        else quitf(_wa, "Incorrect");
    }

    if (partial) quitp(0.5, "Invalid strategy but correct distance");
    else quitf(_ok, "Correct");

}
