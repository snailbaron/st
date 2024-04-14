#include "space.hpp"

#include <arg.hpp>

#include <nlohmann/json.hpp>

#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <string_view>

using json = nlohmann::json;

namespace cmd {



void registerNew(const arg::MultiValue<std::string>& args)
{
    auto parser = arg::Parser{};
    auto symbol = parser.option<std::string>()
        .keys("--symbol")
        .markRequired()
        .help("will become your name in the game");
    auto faction = parser.option<std::string>()
        .keys("--faction")
        .markRequired()
        .help("faction to join, COSMIC is recommended in the tutorial");
    parser.parse(args);

    auto api = space::API{};

    auto token = api.registerNewAgent(*symbol, *faction);
    std::cout << "your token: [" << token << "]\n" <<
        "save it at ~/.space_traders_token file\n";
}

void info(const arg::MultiValue<std::string>& args)
{
    auto parser = arg::Parser{};
    auto object = parser.argument<std::string>()
        .help("object to get info for");
    parser.parse(args);

    auto api = space::API{};

    if (!object.isSet()) {
        api.agentInfo();
        return;
    }

    if (auto waypoint = space::parseWaypoint(object)) {
        api.info(*waypoint);
        return;
    }

    if (auto system = space::parseSystem(object)) {
        api.info(*system);
        return;
    }

    throw e::Error{} << "unknown object: " << object;
}

} // namespace cmd

int main(int argc, char* argv[]) try
{
    auto command = arg::argument<std::string>()
        .metavar("COMMAND")
        .markRequired()
        .help("command to execute");
    auto sink = arg::multiArgument<std::string>();
    arg::helpKeys("-h", "--help");
    arg::parse(argc, argv);

    auto commandMapping = std::map<std::string, void(*)(const arg::MultiValue<std::string>&)>{
        {"register", cmd::registerNew},
        {"info", cmd::info},
    };

    if (auto it = commandMapping.find(command); it != commandMapping.end()) {
        it->second(sink);
        return EXIT_SUCCESS;
    }

    throw e::Error{} << "unknown command: " << command << "\n";
} catch (...) {
    e::handleError();
    return EXIT_FAILURE;
}