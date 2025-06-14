#include <iostream>
#include <optional>
#include <string>

#include "commands.hpp"
#include "config.hpp"
#include "database.hpp"

int main(int argc, char const* argv[])
{
    std::cout << "Running " << argv[0] << " with " << (argc - 1) << " arguments\n";

    // setup the modules for handling stuff
    Config config;
    DataBase db;

    // parse commands
    CommandParser parser(argc, argv);
    parser.package();

    if (parser.failed()) {
        for (auto& err : parser.errors()) {
            std::cout << err << "\n\n";
        }
        std::cout << std::flush;
        return 1;
    }
    if (parser.nothing()) {
        std::cout << cmd::helpString() << std::endl;
        return 1;
    }

    std::unique_ptr<Command> command = parser.collect();

    command->bind(&config);
    command->bind(&db);

    // process the command
    std::expected<void, Error> result = command->execute();

    if (!result) {
        std::cout << result.error() << std::endl;
        return 1;
    }

    std::cout << std::endl;
    return 0;
}
