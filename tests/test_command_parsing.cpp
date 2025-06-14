// stdlib includes
#include <sstream>
#include <string>
#include <vector>
// catch includes
#include <catch2/catch_test_macros.hpp>

#include "commands.hpp"

CommandParser useParser(std::string clText)
{
    // make arc and argv
    std::vector<std::string> args;
    std::vector<char const*> argvPointers;

    std::istringstream iss(clText);
    std::string token;

    // init args with work.exe
    args.push_back("work.exe");
    while (iss >> token) {
        args.push_back(token);
    }

    for (auto& arg : args) {
        argvPointers.push_back(arg.data());
    }

    argvPointers.push_back(nullptr);  // Null-terminate argv

    int argc = static_cast<int>(args.size());
    char const** argv = argvPointers.data();

    CommandParser parser(argc, argv);

    // std::cout << "Created parser with " << argc << " args; ";
    // for (auto& arg : args) std::cout << arg << ", ";

    // std::cout << std::endl;

    return parser;
}

TEST_CASE("Start command useable", "[start]")
{
    SECTION("Start with only time")
    {
        CommandParser parser = useParser("start 09:00");
        parser.package();
        REQUIRE(parser.ok());
        REQUIRE(!parser.nothing());
        std::unique_ptr<Command> c = parser.collect();
        REQUIRE((bool)c);
        REQUIRE(c->type() == Command::Type::Start);
        auto cc = dynamic_cast<StartCommand*>(c.get());
        REQUIRE(nullptr != cc);
        // test the time parsed
        REQUIRE(cc->time.hour == 9);
        REQUIRE(cc->time.minute == 0);
    }

    SECTION("Start with date")
    {
        CommandParser parser = useParser("start fri 10:20");
        parser.package();
        REQUIRE(parser.ok());
        REQUIRE(!parser.nothing());
        std::unique_ptr<Command> c = parser.collect();
        REQUIRE((bool)c);
        REQUIRE(c->type() == Command::Type::Start);
        auto cc = dynamic_cast<StartCommand*>(c.get());
        REQUIRE(nullptr != cc);
        // test the time parsed
        REQUIRE(cc->time.hour == 10);
        REQUIRE(cc->time.minute == 20);
        // test the day
        REQUIRE(cc->date.has_value());
        REQUIRE(cc->date->day == Date::Day::Fri);
    }

    SECTION("Start with bad date")
    {
        CommandParser parser = useParser("start sat 09:23");
        parser.package();
        REQUIRE(parser.failed());
        REQUIRE(parser.errors().size() == 1);
        REQUIRE(parser.errors().at(0).code == Error::Parse);
    }
}

TEST_CASE("End command usage", "[end]")
{
    SECTION("Good end")
    {
        CommandParser parser = useParser("end 19:13");
        parser.package();
        REQUIRE(parser.ok());
        REQUIRE(!parser.nothing());
        std::unique_ptr<Command> c = parser.collect();
        REQUIRE((bool)c);
        REQUIRE(c->type() == Command::Type::End);
        EndCommand* ec = dynamic_cast<EndCommand*>(c.get());
        REQUIRE(nullptr != ec);
        // test the time parsed
        REQUIRE(ec->time.hour == 19);
        REQUIRE(ec->time.minute == 13);
    }
}

TEST_CASE("Break command usage", "[break]")
{
    SECTION("Good break - add break")
    {
        CommandParser parser = useParser("break 19:13");
        parser.package();
        REQUIRE(parser.ok());
        REQUIRE(!parser.nothing());
        std::unique_ptr<Command> c = parser.collect();
        REQUIRE((bool)c);
        REQUIRE(c->type() == Command::Type::Break);
        auto cc = dynamic_cast<AddBreakCommand*>(c.get());
        REQUIRE(nullptr != cc);
        // test the time parsed
        REQUIRE(cc->time.hour == 19);
        REQUIRE(cc->time.minute == 13);
    }
    SECTION("Good break - set break")
    {
        CommandParser parser = useParser("break 12:13 14:50");
        parser.package();
        REQUIRE(parser.ok());
        REQUIRE(!parser.nothing());
        std::unique_ptr<Command> c = parser.collect();
        REQUIRE((bool)c);
        REQUIRE(c->type() == Command::Type::Break);
        auto cc = dynamic_cast<SetBreakCommand*>(c.get());
        REQUIRE(nullptr != cc);
        // test the times parsed
        REQUIRE(cc->start.hour == 12);
        REQUIRE(cc->start.minute == 13);
        REQUIRE(cc->end.hour == 14);
        REQUIRE(cc->end.minute == 50);
    }
}

TEST_CASE("Project command usage", "[project]")
{
    SECTION("Good project - add project")
    {
        CommandParser parser = useParser("project special 4:01");
        parser.package();
        REQUIRE(parser.ok());
        REQUIRE(!parser.nothing());
        std::unique_ptr<Command> c = parser.collect();
        REQUIRE((bool)c);
        REQUIRE(c->type() == Command::Type::Project);
        auto cc = dynamic_cast<ProjectCommand*>(c.get());
        REQUIRE(nullptr != cc);
        // test the parsed variables
        REQUIRE(cc->name == "special");
        REQUIRE(cc->duration.hour == 4);
        REQUIRE(cc->duration.minute == 1);
    }
}

TEST_CASE("Show command usage", "[show]")
{
    SECTION("Good show - add show")
    {
        CommandParser parser = useParser("show");
        parser.package();
        REQUIRE(parser.ok());
        REQUIRE(!parser.nothing());
        std::unique_ptr<Command> c = parser.collect();
        REQUIRE((bool)c);
        REQUIRE(c->type() == Command::Type::Show);
        auto cc = dynamic_cast<ShowCommand*>(c.get());
        REQUIRE(nullptr != cc);
    }
}