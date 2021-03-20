/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// str.hh: header file for string helpers

#ifndef M_STR_HH
#define M_STR_HH

#include <sstream>
#include <stdexcept>
#include <string>

namespace hiemalia {
template <typename T>
T canFromString(const std::string& s) {
    std::istringstream stream(s);
    T value;
    stream >> value;
    return !stream.fail();
}

template <typename T>
T fromString(const std::string& s) {
    std::istringstream stream(s);
    T value;
    stream >> value;
    if (stream.fail()) throw std::runtime_error("failed to convert");
    return value;
}

template <typename T>
std::string toString(const T& value) {
    std::ostringstream stream;
    stream << value;
    return stream.str();
}

template <typename T>
struct c_ify_return {
    using type = T;
};
template <>
struct c_ify_return<std::string> {
    using type = const char*;
};

template <typename T>
typename c_ify_return<T>::type c_ify(T x) {
    return x;
}

template <>
inline typename c_ify_return<std::string>::type c_ify<std::string>(
    std::string s) {
    return s.c_str();
}

template <typename... Ts>
std::string stringFormat(const std::string& fmt, Ts... args) {
    int n = snprintf(nullptr, 0, fmt.c_str(), c_ify<Ts>(args)...);
    if (n < 0) throw std::runtime_error("failed to format");
    std::string s(n, ' ');
    snprintf(s.data(), n + 1, fmt.c_str(), c_ify<Ts>(args)...);
    return s;
}
};  // namespace hiemalia

#endif  // M_STR_HH
