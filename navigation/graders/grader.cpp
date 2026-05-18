#include "navigation.h"

#include <charconv>
#include <csignal>
#include <cstdlib>
#include <cstdio>
#include <concepts>
#include <cstring>
#include <vector>
#include <typeinfo>

// In case of using FIFO, remove this define
#define GRADER_USE_STDIO
constexpr int NUM_PROCS = 2;

namespace
{
    bool exit_allowed = false;
    [[noreturn]] void authorized_exit(int exit_code)
    {
        exit_allowed = true;
        std::exit(exit_code);
    }

    static constexpr int secret_g2m = 0x77B10310;
    static constexpr int secret_m2g = 0XB465B750;
    static constexpr int code_mask = 0x0000000F;

    static constexpr int M2G_CODE__OK = 0;
    static constexpr int M2G_CODE__DIE = 1;

    static constexpr int G2M_CODE__OK = 0;
    static constexpr int G2M_CODE__PV_CALL_EXIT = 13;
    static constexpr int G2M_CODE__PV_TAMPER_M2G = 14;
    static constexpr int G2M_CODE__SILENT = 15;

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
        if (fwrite(static_cast<void *>(&x), sizeof(x), 1, fout) != 1)
        {
            fprintf(stderr, "Could not write %s to fout\n", typeid(x).name());
            authorized_exit(3);
        }
    }

    template <std::integral I>
    void write_int_array(const std::vector<I> &arr)
    {
        write_int(arr.size());
        if (fwrite(static_cast<const void *>(arr.data()), sizeof(I), arr.size(), fout) != arr.size())
        {
            fprintf(stderr, "Could not write %zu of %s to fout\n", arr.size(), typeid(I).name());
            authorized_exit(3);
        }
    }

    void write_secret(int g2m_code = G2M_CODE__OK)
    {
        write_int(secret_g2m | g2m_code);
    }

    [[noreturn]] void die(int g2m_code)
    {
        if (g2m_code == G2M_CODE__OK)
        {
            fprintf(stderr, "Shall not die with code OK\n");
            authorized_exit(5);
        }
        fprintf(stderr, "Dying with code %d\n", g2m_code);
        if (g2m_code != G2M_CODE__SILENT)
            write_secret(g2m_code);
        close_stream(fout);
        close_stream(fin);
        authorized_exit(0);
    }

    template <std::integral I>
    I read_int()
    {
        I x;
        if (fread(static_cast<void *>(&x), sizeof(x), 1, fin) != 1)
        {
            fprintf(stderr, "Could not read %s from fin\n", typeid(x).name());
            authorized_exit(3);
        }
        return x;
    }

    template <std::integral I>
    std::vector<I> read_int_array()
    {
        std::size_t sz = read_int<std::size_t>();
        std::vector<I> arr(sz);
        if (fread(static_cast<void *>(arr.data()), sizeof(I), sz, fin) != sz)
        {
            fprintf(stderr, "Could not read %zu of %s from fin\n", sz, typeid(I).name());
            authorized_exit(3);
        }
        return arr;
    }

    void read_secret()
    {
        int secret = read_int<int>();
        if ((secret & ~code_mask) != secret_m2g)
            die(G2M_CODE__PV_TAMPER_M2G);
        int m2g_code = secret & code_mask;
        if (m2g_code != M2G_CODE__OK)
            die(G2M_CODE__SILENT);
    }

    void check_exit_protocol()
    {
        if (!exit_allowed)
            die(G2M_CODE__PV_CALL_EXIT);
    }

} // namespace

// Returns the id of the process
int parse_args(int argc, char **argv)
{
    int stage = 0;
    {
#ifdef GRADER_USE_STDIO
        int required_args = 1 + (NUM_PROCS != 1);
#else
        int required_args = 3 + (NUM_PROCS != 1);
#endif
        if (argc < required_args)
        {
            fprintf(stderr, "Usage: %s", argv[0]);
#ifndef GRADER_USE_STDIO
            fprintf(stderr, " mgr2sol sol2mgr");
#endif
            if (NUM_PROCS != 1)
                fprintf(stderr, " proc-id");
            fprintf(stderr, "\n");
            authorized_exit(-1);
        }

#ifndef GRADER_USE_STDIO
        fin = fopen(argv[1], "r");
        fout = fopen(argv[2], "w");
#endif

        if (NUM_PROCS != 1)
        {
            char *proc_s = argv[required_args - 1];
            auto [ptr, ec] = std::from_chars(proc_s, proc_s + strlen(proc_s), stage);
            if (ec != std::errc() || ptr != proc_s + strlen(proc_s))
            {
                fprintf(stderr, "Invalid proc-id (not a number)\n");
                authorized_exit(-1);
            }
            if (stage < 0 || stage > NUM_PROCS)
            {
                fprintf(stderr, "Invalid stage-id (should be 0 to NUM_PROCS - 1)\n");
                authorized_exit(-1);
            }
        }
    }
    return stage;
}

int main(int argc, char **argv)
{
    signal(SIGPIPE, SIG_IGN);
    atexit(check_exit_protocol);
    at_quick_exit(check_exit_protocol);

    // Parsing arguments
    int id = parse_args(argc, argv);

    // Grader logic
    if (id == 0)
    {
        while (true)
        {
            read_secret();
            bool keep = read_int<bool>();
            if (!keep)
                break;

            int N = read_int<int>();
            int K = read_int<int>();
            int B = read_int<int>();
            std::vector<int> U = read_int_array<int>();
            std::vector<int> V = read_int_array<int>();
            std::vector<int> P = read_int_array<int>();

            std::vector<int> T = construct_network(N, K, B, U, V, P);
            write_secret();
            write_int_array(T);
            out_flush();
        }
    }
    else
    {
        while (true)
        {
            read_secret();
            bool keep = read_int<bool>();
            if (!keep)
                break;

            int K = read_int<int>();
            int B = read_int<int>();
            std::vector<int> C = read_int_array<int>();

            std::vector<int> D = navigate(K, B, C);
            write_secret();
            write_int_array(D);
            out_flush();
        }
    }
}
