/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// sys.hh: header file for some system functions

#ifndef M_SYS_HH
#define M_SYS_HH

#include <string>

namespace hiemalia {
bool sysOpenConsole();
void sysCloseConsole();
void sysDisplayHelp(const std::string& msg);
void sysDisplayError(const std::string& msg);

};  // namespace hiemalia

#endif  // M_SYS_HH
