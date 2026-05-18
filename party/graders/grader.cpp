#include "party.h"

#include <csignal>
#include <cstdlib>
#include <cstdint>
#include <cstdio>
#include <cassert>
#include <concepts>
#include <cstring>
#include <string>
#include <tuple>
#include <typeinfo>
#include <unistd.h>
#include <vector>

// In case of using FIFO, remove this define
#define GRADER_USE_STDIO

namespace
{
    bool exit_allowed = false;
    [[noreturn]] void authorized_exit(int exit_code)
    {
        exit_allowed = true;
        std::exit(exit_code);
    }

    constexpr int NUM_PROCS = 2;

    static constexpr int secret_g2m = 0xABC86410;
    static constexpr int secret_m2g = 0x97532FF0;
    static constexpr int code_mask = 0x0000000F;

    static constexpr int M2G_CODE_OK = 0;
    static constexpr int M2G_CODE_DIE = 1;

    static constexpr int G2M_CODE_OK = 0;
    static constexpr int G2M_CODE_WA_INVALID_LENGTH = 1;
    static constexpr int G2M_CODE_CANNOT_EXECV = 2;
    static constexpr int G2M_CODE_PV_CALL_EXIT = 13;
    static constexpr int G2M_CODE_PV_TAMPER_M2G = 14;
    static constexpr int G2M_CODE_SILENT = 15;

#ifdef GRADER_USE_STDIO
    FILE *fin = stdin;
    FILE *fout = stdout;
#else
    FILE *fin = nullptr;
    FILE *fout = nullptr;
#endif

    void out_flush()
    {
        fflush(fout);
    }

    void close_stream(FILE *&f)
    {
        if (f)
            fclose(f);
        f = nullptr;
    }

    void write_int(std::integral auto x)
    {
        if (fwrite(reinterpret_cast<char *>(&x), sizeof(x), 1, fout) != 1)
        {
            fprintf(stderr, "Could not write %s to fout\n", typeid(x).name());
            authorized_exit(3);
        }
    }

    void write_secret(int g2m_code = G2M_CODE_OK)
    {
        write_int(secret_g2m | g2m_code);
    }

    [[noreturn]] void die(int g2m_code)
    {
        if (g2m_code == G2M_CODE_OK)
        {
            fprintf(stderr, "Shall not die with code OK\n");
            authorized_exit(5);
        }
        fprintf(stderr, "Dying with code %d\n", g2m_code);
        if (g2m_code != G2M_CODE_SILENT)
            write_secret(g2m_code);
        close_stream(fout);
        close_stream(fin);
        authorized_exit(0);
    }

    template <std::integral I>
    I read_int()
    {
        I x;
        if (fread(reinterpret_cast<char *>(&x), sizeof(x), 1, fin) != 1)
        {
            fprintf(stderr, "Could not read %s from fin\n", typeid(x).name());
            authorized_exit(3);
        }
        return x;
    }

    void read_secret()
    {
        int secret = read_int<int>();
        if ((secret & ~code_mask) != secret_m2g)
            die(G2M_CODE_PV_TAMPER_M2G);
        int m2g_code = secret & code_mask;
        if (m2g_code != M2G_CODE_OK)
            die(G2M_CODE_SILENT);
    }

    void check_exit_protocol()
    {
        if (!exit_allowed)
            die(G2M_CODE_PV_CALL_EXIT);
    }

} // namespace

int main(int argc, char **argv)
{
    std::setvbuf(stdin, nullptr, _IONBF, 0);

    signal(SIGPIPE, SIG_IGN);
    atexit(check_exit_protocol);
    at_quick_exit(check_exit_protocol);

    // Grader logic
    write_secret();
    out_flush();

    read_secret();
    bool cont = read_int<bool>();
    if (!cont)
        authorized_exit(0);
    int N = read_int<int>();
    int K = read_int<int>();
    int p = read_int<int>();
    int r = read_int<int>();

    if (p != -1) { // rooms 0 ~ N - 1
        init(N, K, p, r);
        for (int b = 0; b < N * N; b++) {
            read_secret();
            int f = read_int<int>();
            int s = read_int<int>();
            int x = strategy(b, f, s);

            write_secret();
            write_int(x);
            out_flush();
        }
    } else { // guests outside
        read_secret();
        std::vector<int> F(N * N), S(N * N);
        for (int i = 0; i < N * N; i++)
            F[i] = read_int<int>();
        for (int i = 0; i < N * N; i++)
            S[i] = read_int<int>();

        std::vector<int> Q = guess(N, K, F, S);
        write_secret();
        write_int((int)Q.size());
        for (int i = 0; i < (int)Q.size(); i++)
            write_int(Q[i]);
        out_flush();
    }

    if (access("/proc/self/exe", F_OK) == 0)
        execv("/proc/self/exe", argv);
    write_secret(G2M_CODE_CANNOT_EXECV);
    out_flush();
    authorized_exit(0);
}
