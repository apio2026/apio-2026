#include <algorithm>
#include <cmath>
#include <csignal>
#include <cstdlib>
#include <cstdio>
#include <fstream>
#include <format>
#include <iostream>
#include <iomanip>
#include <random>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

using namespace std::literals::string_literals;

constexpr int NUM_PROCS = 2;

enum class verdicts
{
    judge_failure,
    protocol_violation,
    correct,
    wrong,
    partial
};

[[noreturn]] void quit(verdicts verdict, std::string reason, double score = 0.0)
{
    const char *verdict_str = nullptr;
    int exit_code = EXIT_SUCCESS;

#pragma GCC diagnostic push
#pragma GCC diagnostic error "-Wswitch"
    switch (verdict)
    {
    case verdicts::judge_failure:
        verdict_str = "Judge Failure";
        exit_code = EXIT_FAILURE;
        break;
    case verdicts::protocol_violation:
        verdict_str = "Protocol Violation";
        break;
    case verdicts::correct:
        verdict_str = "translate:success";
        break;
    case verdicts::partial:
        verdict_str = "translate:partial";
        break;
    case verdicts::wrong:
        verdict_str = "translate:wrong";
        break;
    }
#pragma GCC diagnostic pop

    std::cout << std::fixed << std::setprecision(6) << score << std::endl;
    std::cerr << verdict_str << std::endl;
    std::cerr << reason << std::endl;
    std::exit(exit_code);
}
[[noreturn]] void die(verdicts verdict, const char *reason);

struct solution
{
    static constexpr int secret_g2m = 0x77B10310;
    static constexpr int secret_m2g = 0XB465B750;
    static constexpr int code_mask = 0x0000000F;

    static constexpr int M2G_CODE__OK = 0;
    static constexpr int M2G_CODE__DIE = 1;

    static constexpr int G2M_CODE__OK = 0;
    static constexpr int G2M_CODE__PV_CALL_EXIT = 13;
    static constexpr int G2M_CODE__PV_TAMPER_M2G = 14;
    static constexpr int G2M_CODE__SILENT = 15;

    FILE *mgr2sol = nullptr;
    FILE *sol2mgr = nullptr;

    void open(const char *mgr2sol_filename, const char *sol2mgr_filename)
    {
        // In this order
        mgr2sol = fopen(mgr2sol_filename, "a");
        if (!mgr2sol)
            quit(verdicts::judge_failure, "Could not open file "s + mgr2sol_filename);

        sol2mgr = fopen(sol2mgr_filename, "r");
        if (!sol2mgr)
            quit(verdicts::judge_failure, "Could not open file "s + sol2mgr_filename);
    }

    solution() {}
    solution(const char *mgr2sol_filename, const char *sol2mgr_filename)
    {
        open(mgr2sol_filename, sol2mgr_filename);
    }

    void close()
    {
        if (mgr2sol)
            fclose(mgr2sol), mgr2sol = nullptr;
        if (sol2mgr)
            fclose(sol2mgr), sol2mgr = nullptr;
    }

    void flush()
    {
        if (mgr2sol)
            fflush(mgr2sol);
    }

    void write(std::integral auto x)
    {
        if (mgr2sol == nullptr || fwrite(static_cast<void *>(&x), sizeof(x), 1, mgr2sol) != 1)
            close();
    }

    template <std::integral I>
    void write_array(const std::vector<I> &arr)
    {
        write(arr.size());
        if (mgr2sol == nullptr || fwrite(static_cast<const void *>(arr.data()), sizeof(I), arr.size(), mgr2sol) != arr.size())
            close();
    }

    void write_secret(int m2g_code = M2G_CODE__OK)
    {
        write(secret_m2g | m2g_code);
    }

    void read_secret()
    {
        int secret = read<int>();
        if ((secret & ~code_mask) != secret_g2m)
            die(verdicts::protocol_violation, "Possible tampering with sol2mgr");
        int g2m_code = secret & code_mask;
        switch (g2m_code)
        {
        case G2M_CODE__OK:
            return;
        case G2M_CODE__SILENT:
            quit(verdicts::judge_failure, "Unexpected g2m_code SILENT from sol2mgr");
        case G2M_CODE__PV_TAMPER_M2G:
            die(verdicts::protocol_violation, "Possible tampering with mgr2sol");
        case G2M_CODE__PV_CALL_EXIT:
            die(verdicts::protocol_violation, "Solution called exit()");
        default:
            quit(verdicts::judge_failure, "Unknown g2m_code "s + std::to_string(g2m_code) + " from sol2mgr");
        }
    }

    template <std::integral T>
    T read()
    {
        T x;
        if (sol2mgr == nullptr || fread(static_cast<void *>(&x), sizeof(x), 1, sol2mgr) != 1)
        {
            close();
            die(verdicts::protocol_violation, "manual PV, can't read int from grader");
        }
        return x;
    }

    std::vector<int> read_array(std::size_t expected_size, const char *reason)
    {
        std::vector<int> x(expected_size);
        if (read<std::size_t>() != expected_size)
            die(verdicts::wrong, reason);

        if (sol2mgr == nullptr || fread(reinterpret_cast<char *>(x.data()), sizeof(int), expected_size, sol2mgr) != expected_size)
        {
            close();
            die(verdicts::protocol_violation, "manual PV, can't read int array from grader");
        }
        return x;
    }
} sol[NUM_PROCS];

[[noreturn]] void die(verdicts verdict, const char *reason)
{
    for (int i = 0; i < NUM_PROCS; i++)
        sol[i].write_secret(solution::M2G_CODE__DIE);
    quit(verdict, reason);
}

void init_communication(int argc, char **argv)
{
    std::signal(SIGPIPE, SIG_IGN);
    {
        int required_args = 1 + 2 * NUM_PROCS;
        if (argc < required_args || required_args + 1 < argc)
        {
            std::string usage = argv[0];
            for (int i = 0; i < NUM_PROCS; i++)
                usage += std::format(" sol{0}-to-mgr mgr-to-sol{0}", i);

            std::cerr << "Invalid number of arguments: " << argc << std::endl
                      << "Usage: " << usage << std::endl;
            std::exit(EXIT_FAILURE);
        }
    }
    for (int i = 0; i < NUM_PROCS; i++)
        sol[i].open(argv[2 * i + 2], argv[2 * i + 1]);
}

// Judge libs

template <typename T>
concept PRNG = requires(T rng) {
    { rng() } -> std::convertible_to<unsigned int>;
};

// returns uniform number in [0, r]
// https://arxiv.org/pdf/1805.10941
unsigned int uniform_distrib(unsigned r, PRNG auto &rng)
{
    unsigned x = rng();
    auto m = (unsigned long long)x * r;
    unsigned l = m;
    if (l < r)
    {
        unsigned t = -r % r;
        while (l < t)
        {
            x = rng();
            m = (unsigned long long)x * r;
            l = m;
        }
    }
    return (unsigned)(m >> 32);
}

template <typename T>
void shuffle(std::vector<T> &arr, PRNG auto &rng)
{
    int n = (int)arr.size();
    for (int i = n - 1; i >= 1; i--)
    {
        int j = uniform_distrib(i + 1, rng);
        if (i != j)
            std::swap(arr[i], arr[j]);
    }
}

// Judge strategies

constexpr int MAX_T = 100;

inline int read_int_from_stdin(const std::string &variable_name)
{
    int x;
    if (scanf("%d", &x) != 1)
        quit(verdicts::judge_failure, "Cannot read " + variable_name + " from stdin");
    return x;
}

int main(int argc, char **argv)
{
    // Prepare solutions
    init_communication(argc, argv);

    // Communication
    auto &net = sol[0], &bot = sol[1];

    char subtask;
    if (scanf("%c", &subtask) != 1)
        quit(verdicts::judge_failure, "subtask");

    int seed = read_int_from_stdin("random seed");
    std::mt19937 rng(seed);

    int num_case = read_int_from_stdin("T (#scenarios)");
    int B = read_int_from_stdin("B");
    int K = read_int_from_stdin("K");

    int S = 0;


    std::vector<std::pair<std::vector<int>, std::vector<int>>> queries;
    // collect all possible calls
    for (int scenario = 0; scenario < num_case; scenario++)
    {
        int N = read_int_from_stdin(std::format("N (scenario {})", scenario));

        std::vector<int> U(N - 1), V(N - 1);
        for (int i = 0; i < N - 1; i++)
        {
            U[i] = read_int_from_stdin(std::format("U_{} (scenario {})", i, scenario));
            V[i] = read_int_from_stdin(std::format("V_{} (scenario {})", i, scenario));
        }

        std::vector<int> P(K);
        for (int i = 0; i < K; i++)
            P[i] = read_int_from_stdin(std::format("P_{} (scenario {})", i, scenario));

        int Q;
        if (scanf("%d", &Q) != 1)
            quit(verdicts::judge_failure, std::format("Q (scenario {})", scenario));

        std::vector<int> X(Q);
        for (int i = 0; i < Q; i++)
            X[i] = read_int_from_stdin(std::format("X_{} (scenario {})", i, scenario));

        // construct_network
        net.write_secret();
        net.write(true);
        net.write(N);
        net.write(K);
        net.write(B);
        net.write_array(U);
        net.write_array(V);
        net.write_array(P);
        net.flush();

        net.read_secret();
        auto T = net.read_array(N - 1, "returned array T is not size N - 1");
        for (auto t : T)
            if (t < 0 || t >= MAX_T)
                die(verdicts::wrong, "T contains invalid entry");
        S = std::max(S, 1 + *std::max_element(T.begin(), T.end()));

        // compute
        std::vector<int> subtree(N, 0);
        std::vector<int> parent(N, -1);

        std::vector<std::vector<std::pair<int, int>>> adj(N);
        for (int i = 0; i < N - 1; i++)
        {
            adj[U[i]].emplace_back(V[i], T[i]);
            adj[V[i]].emplace_back(U[i], T[i]);
        }
        for (int i = 0; i < K; i++)
            subtree[P[i]]++;

        auto dfs = [&](auto &self, int u, int p) -> void
        {
            for (const auto &[v, t] : adj[u])
                if (v != p)
                {
                    parent[v] = u;
                    self(self, v, u);
                    subtree[u] += subtree[v];
                }
        };
        dfs(dfs, 0, -1);

        for (int i = 0; i < Q; i++)
        {
            int x = X[i];
            std::vector<std::pair<int, int>> actual;
            for (auto &[v, c] : adj[x])
            {
                int sub = (v == parent[x] ? K - subtree[x] : subtree[v]);
                actual.emplace_back(sub, c);
            }
            if (subtask != '0')
                ::shuffle(actual, rng);

            std::vector<int> C(actual.size()), D(actual.size());
            for (size_t i = 0; i < actual.size(); i++)
                std::tie(D[i], C[i]) = actual[i];
            queries.emplace_back(C, D);
        }
    }
    net.write_secret();
    net.write(false);

    std::vector<int> order(queries.size());
    std::iota(order.begin(), order.end(), 0);
    if (subtask != '0')
        shuffle(order, rng);

    for (int i : order)
    {
        const auto &[C, D] = queries[i];
        bot.write_secret();
        bot.write(true);
        bot.write(K);
        bot.write(B);
        bot.write_array(C);
        bot.flush();

        bot.read_secret();
        auto bot_D = bot.read_array(C.size(), "returned array size D != size C");
        if (D != bot_D)
            die(verdicts::wrong, "Robot returned incorrect D");
    }

    bot.write_secret();
    bot.write(false);

    if (subtask == '0')
        quit(verdicts::correct, std::format("S = {}", S), 1.0);
    if (subtask == '1')
    {
        if (S <= 5)
            quit(verdicts::correct, std::format("S = {}", S), 1.0);
        else if (S <= 100)
            quit(verdicts::partial, std::format("S = {}", S), 2.0 / 6.0);
    }
    if (subtask == '2')
    {
        if (S <= 5)
            quit(verdicts::correct, std::format("S = {}", S), 1.0);
        else if (S == 6)
            quit(verdicts::partial, std::format("S = {}", S), 10.0 / 14.0);
        else if (S <= 9)
            quit(verdicts::partial, std::format("S = {}", S), 6.0 / 14.0);
        else if (S <= 15)
            quit(verdicts::partial, std::format("S = {}", S), 4.0 / 14.0);
        else if (S <= 100)
            quit(verdicts::partial, std::format("S = {}", S), 2.0 / 14.0);
    }
    if (subtask == '3')
    {
        if (S <= 4)
            quit(verdicts::correct, std::format("S = {}", S), 1.0);
        else if (S == 5)
            quit(verdicts::partial, std::format("S = {}", S), 17.5 / 20.0);
        else if (S == 6)
            quit(verdicts::partial, std::format("S = {}", S), 15.0 / 20.0);
        else if (S <= 9)
            quit(verdicts::partial, std::format("S = {}", S), (21.0 - S) / 20.0);
        else if (S <= 15)
            quit(verdicts::partial, std::format("S = {}", S), (16.0 - 0.5 * S) / 20.0);
        else if (S <= 100)
            quit(verdicts::partial, std::format("S = {}", S), (12.0 - log2(S)) / 20.0);
    }
    if (subtask == '5')
    {
        if (S <= 5)
            quit(verdicts::correct, std::format("S = {}", S), 1.0);
        else if (S == 6)
            quit(verdicts::partial, std::format("S = {}", S), 30.0 / 40.0);
        else if (S <= 9)
            quit(verdicts::partial, std::format("S = {}", S), (42.0 - 2 * S) / 40.0);
        else if (S <= 15)
            quit(verdicts::partial, std::format("S = {}", S), (32.0 - S) / 40.0);
        else if (S <= 100)
            quit(verdicts::partial, std::format("S = {}", S), (24.0 - 2.0 * log2(S)) / 40.0);
    }

    quit(verdicts::judge_failure, "Reached an unreachable code!");
    return 0;
}
