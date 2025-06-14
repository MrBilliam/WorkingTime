#pragma once

#include <charconv>
#include <expected>
#include <string>
#include <string_view>

#include "errors.hpp"

/**
 * simple struct for recordable time.
 */
struct Time {
    unsigned int hour = 0;
    unsigned int minute = 0;
};

struct Date {
    enum class Day { None = 0, Mon, Tue, Wed, Thurs, Fri, SIZE };

    unsigned int weekno = 0;
    Day day;
};

std::expected<Time, Error> TimeFromString(std::string_view str)
{
    std::size_t colonPos = str.find(':');
    if (std::string_view::npos == colonPos)
        return std::unexpected(Error{Error::Parse, "No time separator found."});

    std::from_chars_result result;

    Time out;

    // Parse hours
    result = std::from_chars(str.data(), str.data() + colonPos, out.hour);
    if (result.ec != std::errc{})
        return std::unexpected(Error{Error::Parse, "Could not parse hours."});

    // Parse minutes
    result = std::from_chars(str.data() + colonPos + 1, str.data() + str.size(), out.minute);
    if (result.ec != std::errc{})
        return std::unexpected(Error{Error::Parse, "Could not parse minutes."});

    return out;
}

std::expected<Date, Error> DateFromString(std::string_view str)
{
    // try and match the day, then work out the week num from the current datetime.
    Date rtn;

    if (str.contains("mon"))
        rtn.day = Date::Day::Mon;
    else if (str.contains("tue"))
        rtn.day = Date::Day::Tue;
    else if (str.contains("wed"))
        rtn.day = Date::Day::Wed;
    else if (str.contains("thurs"))
        rtn.day = Date::Day::Thurs;
    else if (str.contains("fri"))
        rtn.day = Date::Day::Fri;
    else
        return std::unexpected(Error{Error::Parse, "Unrecognised day"});

    return rtn;
}

class DataBase {
   public:
    DataBase()
    {
    }
};