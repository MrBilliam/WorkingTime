#pragma once

#include <array>
#include <chrono>
#include <expected>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include "config.hpp"
#include "database.hpp"

class Command {
   public:
    /**
     * Enum for kinds of commands possible.
     */
    enum class Type : unsigned int {
        None = 0,
        Start,  // start the day
        End,    // end the day
        Break,
        Project,  // log a project: log banter 1:25
        Show,     // show a selection: show week (blank: this-week, #num: a particular week)
        SIZE
    };

    explicit Command(Type t) : m_type(t)
    {
    }

    virtual ~Command()
    {
    }

    virtual void bind(Config* cfg)
    {
        m_config = cfg;
    }
    virtual void bind(DataBase* db)
    {
        m_database = db;
    };

    /**
     * virtual function to execute the commands actions. Might return an error.
     */
    virtual std::expected<void, Error> execute()
    {
        return {};
    }

    inline Type type() const
    {
        return m_type;
    }

   protected:
    Config* m_config = nullptr;
    DataBase* m_database = nullptr;

   private:
    Type m_type;
};

namespace cmd {
constexpr std::string_view toString(Command::Type ck)
{
    switch (ck) {
        case Command::Type::Start:
            return "start";
        case Command::Type::End:
            return "end";
        case Command::Type::Break:
            return "break";
        case Command::Type::Project:
            return "project";
        case Command::Type::Show:
            return "show";
        default:
            break;
    }

    return "";
}

constexpr Command::Type match(std::string_view sv)
{
    if (sv == "start") return Command::Type::Start;
    if (sv == "end") return Command::Type::End;
    if (sv == "break") return Command::Type::Break;
    if (sv == "project") return Command::Type::Project;
    if (sv == "show") return Command::Type::Show;

    return Command::Type::None;
}

constexpr std::array<std::string, (std::size_t)Command::Type::SIZE> allCommands()
{
    std::array<std::string, (std::size_t)Command::Type::SIZE> rtn;
    for (std::underlying_type_t<Command::Type> c = 0; std::string& val : rtn) {
        val = toString((Command::Type)++c);
    }

    return rtn;
}

constexpr std::string helpString()
{
    return "Usage: work <command>\n"
           "\nCommands:\n"
           "  start   <?day> <time>       Starts the clocking\n"
           "  end    <time>               Stops the clocking\n"
           "  break   <time> <?end-time>  Input a break\n"
           "  project <name> <time>       Log a project timeing\n"
           "  show                        Show the breakdown of the current week\n"
           "\nOptions:\n"
           "  -h, --help                  Show this help message and exit\n"
           "\nExamples:\n"
           "  work start 8:15\n"
           "  work end 17:12\n"
           "  work break 12:00\n"
           "  work break 12:20 14:00\n";
}
}  // namespace cmd

// ---- Commands ----
// NOTE: i've made the member variables public for the unit tests, and the usage of commands isn't
// that extensive in this app.

class StartCommand : public Command {
   public:
    std::optional<Date> date;
    Time time;

    explicit StartCommand(Time&& t) : Command(Type::Start), time(t)
    {
    }

    void addDate(Date&& d)
    {
        date = d;
    }
};

class EndCommand : public Command {
   public:
    Time time;

    explicit EndCommand(Time&& t) : Command(Type::End), time(t)
    {
    }
};

class AddBreakCommand : public Command {
   public:
    Time time;

    explicit AddBreakCommand(Time&& t) : Command(Type::Break), time(t)
    {
    }
};

class SetBreakCommand : public Command {
   public:
    Time start;
    Time end;

    SetBreakCommand(Time&& a, Time&& b) : Command(Type::Break), start(a), end(b)
    {
    }
};

class ProjectCommand : public Command {
   public:
    std::string name;
    Time duration;

    ProjectCommand(std::string&& n, Time&& t) : Command(Type::Project), name(n), duration(t)
    {
    }
};

class ShowCommand : public Command {
   public:
    ShowCommand() : Command(Type::Show)
    {
    }
};

/**
 * a command creator from the command line arguments
 */
class CommandParser {
   private:  // member variables
    std::unique_ptr<Command> m_command;

    Command::Type m_ctype;
    std::vector<std::string> m_args;
    std::vector<std::string> m_flags;

    std::vector<Error> m_errors;

   public:  // constructors
    CommandParser(int argc, char const* argv[])
    {
        if (argc < 2) return;

        // 1. what kind of command
        m_ctype = cmd::match(argv[1]);
        if (Command::Type::None == m_ctype) return;

        // 2. collect the args and flags
        for (int i = 2; i < argc; ++i) {
            std::string_view value = argv[i];

            if (value.starts_with('-'))
                m_flags.emplace_back(value);
            else
                m_args.emplace_back(value);
        }
    }

   public:  // methods
    /**
     * once the parser has been initialised it needs to create the command object. This is where
     * that occurs.
     */
    CommandParser& package()
    {
        // use an iterator to step through the args
        auto args_itr = m_args.cbegin();

        switch (m_ctype) {
            case Command::Type::Start: {
                if (m_args.cend() == args_itr) {
                    m_errors.emplace_back(Error::InsufficientArgs, "Expected a time.");
                    break;
                }
                // see if there's a date first
                auto date = DateFromString(*args_itr);
                if (date) ++args_itr;
                // else don't worry if errored.

                // we must have a time.
                if (m_args.cend() == args_itr) {
                    m_errors.emplace_back(Error::InsufficientArgs,
                                          "Expected time as well as date.");
                    break;
                }
                auto time = TimeFromString(*args_itr);
                // now create the start command
                if (!time) {
                    m_errors.push_back(std::move(time.error()));
                    break;
                }
                auto command = std::make_unique<StartCommand>(std::move(*time));
                if (date) {
                    command->addDate(std::move(*date));
                }
                m_command = std::move(command);
            } break;

            case Command::Type::End: {
                if (m_args.cend() == args_itr) {
                    m_errors.emplace_back(Error::InsufficientArgs, "Expected a time.");
                    break;
                }
                auto time = TimeFromString(m_args.at(0));
                m_command = std::make_unique<EndCommand>(std::move(*time));
            } break;

            case Command::Type::Break: {
                if (m_args.cend() == args_itr) {
                    m_errors.emplace_back(Error::InsufficientArgs, "Expected a time.");
                    break;
                }
                // note: there are two options here: set and add
                auto time = TimeFromString(*args_itr);

                if (!time) {
                    m_errors.push_back(std::move(time.error()));
                    break;
                }
                // make the command
                if (m_args.cend() == ++args_itr) {  // if there might be a second time
                    m_command = std::make_unique<AddBreakCommand>(std::move(*time));
                    break;
                }
                auto time2 = TimeFromString(*args_itr);
                if (!time2) {
                    m_errors.push_back(std::move(time2.error()));
                    break;
                }
                m_command = std::make_unique<SetBreakCommand>(std::move(*time), std::move(*time2));
            } break;

            case Command::Type::Project: {
                if (m_args.cend() == args_itr) {
                    m_errors.emplace_back(Error::InsufficientArgs, "Expected a project name.");
                    break;
                }
                // can just set the project from the args
                std::string name = std::move(*args_itr);

                // try duration
                if (m_args.cend() == ++args_itr) {
                    m_errors.emplace_back(Error::InsufficientArgs, "Expected a duration time.");
                    break;
                }
                auto time = TimeFromString(*args_itr);
                if (!time) {
                    m_errors.push_back(std::move(time.error()));
                    break;
                }
                m_command = std::make_unique<ProjectCommand>(std::move(name), std::move(*time));
            } break;

            case Command::Type::Show: {
                m_command = std::make_unique<ShowCommand>();
            } break;

            default:
                break;
        }

        return *this;
    }

    /**
     * getter of the parsed command.
     */
    inline std::unique_ptr<Command> collect()
    {
        return std::move(m_command);
    }

    /**
     * getter of errors.
     */
    inline const std::vector<Error>& errors() const
    {
        return m_errors;
    }

    /**
     * query if no commands were obtained.
     */
    inline bool nothing() const
    {
        return !m_command || Command::Type::None == m_command->type();
    }

    inline bool failed() const
    {
        return m_errors.size() != 0;
    }
    inline bool ok() const
    {
        return m_errors.size() == 0;
    }
};
