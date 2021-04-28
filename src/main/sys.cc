/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// sys.cc: implementation of some system functions

#include "sys.hh"

#include <iostream>

#include "defs.hh"
#include "hbase.hh"
#ifdef _WIN32
#include "Windows.h"
#endif

namespace hiemalia {
bool sysOpenConsole() {
#ifdef _WIN32
    if (AllocConsole()) {
        FILE* tmp;
        freopen_s(&tmp, "CONIN$", "r", stdin);
        freopen_s(&tmp, "CONOUT$", "w", stderr);
        freopen_s(&tmp, "CONOUT$", "w", stdout);
        std::cout.clear();
        std::clog.clear();
        std::cerr.clear();
        std::cin.clear();
        std::ios::sync_with_stdio();
        return true;
    }
#else
    // nothing, sorry
#endif
    return false;
}

void sysCloseConsole() {
#ifdef _WIN32
    std::cerr << "Keeping console open... press ENTER to close" << std::endl;
    std::cin.get();
    FreeConsole();
#else
    // nothing, sorry
#endif
}

void sysDisplayHelp(const std::string& msg) {
#ifdef _WIN32
    MessageBoxA(NULL, msg.c_str(), gameTitle.c_str(), MB_OK | MB_ICONASTERISK);
#else
    std::cerr << msg << std::endl;
#endif
}

void sysDisplayError(const std::string& msg) {
#ifdef _WIN32
    MessageBoxA(NULL, msg.c_str(), gameTitle.c_str(), MB_OK | MB_ICONHAND);
#else
    hostDisplayError(gameTitle, msg);
#endif
}
}  // namespace hiemalia

int main(int argc, char* argv[]);

#ifdef _WIN32
int APIENTRY WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPSTR lpCmdLine, _In_ int nShowCmd) {
    return main(__argc, __argv);
}
#endif
