#pragma once

#include <iostream>
#include <string>

/**
 * type for errors in working-time application
 */
class Error {
   public:
    enum Kind { None, InvalidCommand, Parse, InsufficientArgs };

    Kind code;
    std::string message;

    constexpr Error() = default;
    constexpr Error(Kind c, const std::string& m) : code(c), message(m)
    {
    }
    constexpr Error(const Error& o) = default;
    constexpr Error(Error&& o) = default;

    constexpr Error& operator=(const Error& o) = default;
    constexpr Error& operator=(Error&& o) = default;
};

std::ostream& operator<<(std::ostream& os, const Error& err)
{
    os << "[";
    switch (err.code) {
        case Error::None:
            os << "None";
            break;
        case Error::Parse:
            os << "Parse";
            break;
        default:
            os << "Unknown";
            break;
    }
    os << "-Error] - " << err.message;
    return os;
}
