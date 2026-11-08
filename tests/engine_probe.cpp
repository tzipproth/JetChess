#include "../MoveGen/mgen6.h"
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <atomic>
#include <chrono>
#include <thread>

// A separate test executable, deliberately not a third project in JetChess.sln.
int main(int argc, char** argv)
{
    if (argc == 2 && std::string(argv[1]) == "--threaded") {
        set_winflag(1);
        init_tables();
        init_mytime();
        for (int mode = 1; mode <= 4; ++mode) {
            if (set_opmode(mode) <= 0) return 10;
            for (int attempt = 0; attempt < 4; ++attempt) {
                set_stopflag(0);
                std::atomic<bool> finished{false};
                std::thread worker([&] {
                    Anzahl_DoAll(12, get_fen(0), 1);
                    finished = true;
                });
                if (attempt > 0) std::this_thread::sleep_for(std::chrono::milliseconds(10));
                set_stopflag(3);
                const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(5);
                while (!finished) {
                    get_anztext(); get_text(0); get_anzinfo(); get_info(); get_anzresult(); get_result();
                    if (std::chrono::steady_clock::now() > deadline) std::abort();
                    std::this_thread::yield();
                }
                worker.join();
                if (get_stopflag() != 3) return 11;
                set_stopflag(1);
            }
        }
        set_opmode(1);
        Anzahl_DoAll(3, get_fen(0), 1);
        if (std::string(get_result()).find("8.902") == std::string::npos) return 12;
        std::cout << "16 concurrent cancellation/read cases and restart passed\n";
        return 0;
    }
    if (argc < 4) return 2;
    std::ifstream input(argv[1]);
    std::string fen;
    std::getline(input, fen);
    if (fen.empty()) return 3;
    set_winflag(1);
    init_tables();
    init_mytime();
    const int mode = std::string("MPSB").find(argv[2][0]) + 1;
    if (mode < 1 || mode > 4) return 4;
    int allocated=set_opmode(mode);
    if (allocated <= 0) return 5;
    if (argc > 4 && (allocated!=std::atoi(argv[4]) && hash_set(std::atoi(argv[4]))!=std::atoi(argv[4]))) return 6;
    Anzahl_DoAll(std::atoi(argv[3]), fen.data(), argc > 5 ? std::atoi(argv[5]) : 1);
    std::cout << get_result() << '\n' << get_info() << '\n';
    for (int i = 0; i < get_anztext(); ++i) std::cout << get_text(i) << '\n';
    return 0;
}
