#include <algorithm>
#include <csignal>
#include <cstdlib>
#include <fstream>
#include <format>
#include <iostream>
#include <iomanip>
#include <vector>
#include <random>

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
    static constexpr int secret_g2m = 0x7F6BA410;
    static constexpr int secret_m2g = 0XCB7489D0;
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
        if (mgr2sol) fflush(mgr2sol);
    }

    void write(std::integral auto x)
    {
        if (mgr2sol == nullptr || fwrite(reinterpret_cast<char *>(&x), sizeof(x), 1, mgr2sol) != 1)
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
        if (sol2mgr == nullptr || fread(reinterpret_cast<char *>(&x), sizeof(x), 1, sol2mgr) != 1)
        {
            close();
            die(verdicts::protocol_violation, "manual PV, can't read int from grader");
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

// Judge strategies

constexpr int MAX_LENGTH = 120;

int main(int argc, char **argv)
{
    // Prepare solutions
    init_communication(argc, argv);

    // Communication
    auto &alice = sol[0], &bob = sol[1];

    int n, k;
    if (!(std::cin >> n >> k))
        quit(verdicts::judge_failure, "Cannot read n and k from input");

    std::vector <int> nums;
    for (int i = 0; i < n; ++i) {
        int x;
        if (!(std::cin >> x))
            quit(verdicts::judge_failure, "Cannot read x from input");
        nums.push_back(x);
    }

    alice.write_secret();
    alice.write(k);
    alice.flush();
    bob.write_secret();
    bob.write(k);
    bob.flush();

    std::vector <std::string> enc;
    std::mt19937 rng(56562);
    for (int i = 0; i < n; ++i) {
        alice.write_secret();
        alice.write(true);
        alice.write(nums[i]);
        alice.flush();

        alice.read_secret();
        int len = alice.read<int>();
        if (len > MAX_LENGTH)
            die(verdicts::wrong, "Encoding exceeds maximum allowed length");
        if (len < 0)
            die(verdicts::wrong, "Encoding returns invalid length");

        std::string msg(len, '\0');
        for (auto &c : msg)
            c = alice.read<char>();
        for (auto c : msg)
            if (c != 'O' && c != 'I')
                die(verdicts::wrong, "Invalid encoding");
        
        int times = std::min(std::uniform_int_distribution<int>(0, 1000)(rng), k);
        for (int iter = 0; iter < times; ++iter) {
            std::vector <int> pos;
            for (int j = 0; j < len - 1; ++j) {
                if (msg[j] == 'O' && msg[j + 1] == 'I') pos.push_back(j);
            }
            if (pos.empty()) break;
            int idx = std::uniform_int_distribution<int>(0, (int)pos.size() - 1)(rng);
            std::swap(msg[pos[idx]], msg[pos[idx] + 1]);
        }
        enc.push_back(msg);
    }

    alice.write_secret();
    alice.write(false);
    alice.flush();

    std::vector <int> ord(n);
    std::iota(ord.begin(), ord.end(), 0);
    shuffle(ord.begin(), ord.end(), rng);

    for (int t = 0; t < n; ++t) {
        int i = ord[t];
        bob.write_secret();
        bob.write(true);
        bob.write((int)enc[i].size());
        for (const auto &c : enc[i])
            bob.write(c);
        bob.flush();

        bob.read_secret();
        int nums_recovered = bob.read<int>();
        if (nums[i] != nums_recovered)
            die(verdicts::wrong, "Incorrect recovered x");
    }

    bob.write_secret();
    bob.write(false);
    bob.flush();

    int max_len = 0;
    for (int i = 0; i < n; ++i) {
        max_len = std::max(max_len, (int)enc[i].size());
    }

    if (max_len <= 46) {
        quit(verdicts::correct, std::format("Max length = {}", max_len), 1.0);
    } else if (max_len <= 60) {
        quit(verdicts::partial, std::format("Max length = {}", max_len), 0.44 + 0.04 * (60 - max_len));
    } else {
        quit(verdicts::partial, std::format("Max length = {}", max_len), 0.04 + 2.0 / 3 * (120 - max_len));
    }
    
    quit(verdicts::judge_failure, "Reached an unreachable code!");
    return 0;
}
