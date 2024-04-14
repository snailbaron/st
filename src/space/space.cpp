#include "space.hpp"

#include <cstdlib>
#include <filesystem>
#include <format>
#include <fstream>
#include <regex>

namespace space {

namespace {

const auto baseUrl = http::URL{"https://api.spacetraders.io/v2"};

std::filesystem::path home()
{
#ifdef _WIN32
    return std::getenv("HOMEPATH");
#endif
}

std::string token()
{
    auto tokenFile = std::ifstream{home() / ".space_traders_token"};
    tokenFile.exceptions(std::ios::badbit | std::ios::failbit);
    auto token = std::string{};
    tokenFile >> token;
    tokenFile.close();
    return token;
}

std::pair<std::string, std::string> authHeader()
{
    return {"Authorization", std::format("Bearer {}", token())};
}

} // namespace

std::string System::fullName() const
{
    return std::format("{}-{}", sector, system);
}

std::ostream& operator<<(std::ostream& output, const System& system)
{
    return output << system.sector << "-" << system.system;
}

std::optional<System> parseSystem(const std::string& string)
{
    static const auto systemRegex = std::regex{"([A-Z0-9]+)-([A-Z0-9]+)"};

    auto match = std::smatch{};
    if (std::regex_match(string, match, systemRegex)) {
        return System{
            .sector = match[1],
            .system = match[2],
        };
    }
    return std::nullopt;
}

std::string Waypoint::fullSystemName() const
{
    return std::format("{}-{}", sector, system);
}

std::string Waypoint::fullName() const
{
    return std::format("{}-{}-{}", sector, system, point);
}

std::ostream& operator<<(std::ostream& output, const Waypoint& waypoint)
{
    return output <<
        waypoint.sector << "-" << waypoint.system << "-" << waypoint.point;
}

std::optional<Waypoint> parseWaypoint(const std::string& string)
{
    static const auto waypointRegex =
        std::regex{"([A-Z0-9]+)-([A-Z0-9]+)-([A-Z0-9]+)"};

    auto match = std::smatch{};
    if (std::regex_match(string, match, waypointRegex)) {
        return Waypoint{
            .sector = match[1],
            .system = match[2],
            .point = match[3],
        };
    }
    return std::nullopt;
}

std::string API::registerNewAgent(std::string_view symbol, std::string_view faction)
{
    auto r = _session(http::Request{
        .url = baseUrl / "register",
        .json = {
            {"symbol", symbol},
            {"faction", faction},
        },
    });

    return r.json()["data"]["token"].get<std::string>();
}

void API::agentInfo()
{
    auto r = _session(http::Request{
        .url = baseUrl / "my/agent",
        .headers = {authHeader()},
    });

    std::cout << r.json()["data"].dump(4) << "\n";
}

void API::info(const Waypoint& waypoint)
{
    auto r = _session(http::Request{
        .url = baseUrl / "systems" / waypoint.fullSystemName() /
            "waypoints" / waypoint.fullName(),
        .headers = {authHeader()},
    });

    std::cout << r.json()["data"].dump(4) << "\n";
}

void API::info(const System& system)
{
    auto r = _session(http::Request{
        .url = baseUrl / "systems" / system.fullName() / "waypoints",
        .headers = {authHeader()},
    });

    auto data = r.json()["data"];

    for (const auto& object : data) {
        std::cout << "  * " << object["symbol"].get<std::string>() <<
            ": " << object["type"].get<std::string>() <<
            " @ " << object["x"] << ", " << object["y"] << "\n";
        if (object["isUnderConstruction"]) {
            std::cout << "under construction\n";
        }
        if (const auto& modifiers = object["modifiers"]; !modifiers.empty()) {
            std::cout << "    modifiers: " << modifiers << "\n";
        }
        if (const auto& orbitals = object["orbitals"]; !orbitals.empty()) {
            std::cout << "    orbitals: " << orbitals << "\n";
        }
        std::cout << "    traits:";
        for (const auto& trait : object["traits"]) {
            std::cout << " " << trait["symbol"].get<std::string>();
        }
        std::cout << "\n";

        std::cout << "\n";
    }
}

} // namespace space