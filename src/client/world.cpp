#include "world.hpp"

#include <fs.hpp>

#include <format>
#include <utility>
#include <vector>
#include <set>

namespace {

const auto url = http::URL{"https://api.spacetraders.io/v2"};

} // namespace

World::World()
    : _token(fs::readText(fs::home() / ".space_traders_token"))
{
    //auto agentData = _http(http::Request{
    //    .url = url / "my/agent",
    //    .headers = {authHeader()},
    //});

    //auto json = agentData.json();
    //auto headquarters = Waypoint{json["headquarters"]};

    auto systemsJson = _http(http::Request{
        .url = url / "systems",
        .headers = {authHeader()},
    }).json();

    const auto& systems = systemsJson["data"];
    for (const auto& s : systems) {
        auto system = System{
            .symbol = s["symbol"],
            .sectorSymbol = s["sectorSymbol"],
            .type = systemType(s["type"]),
            .point = Point<float>{s["x"], s["y"]},
            .waypoints = {},
        };

        for (const auto& w : s["waypoints"]) {
            auto waypoint = Waypoint{
                .symbol = w["symbol"],
                .type = fromString<WaypointType>(w["type"]),
                .point = Point<float>{w["x"], w["y"]},
                .orbitals = {},
            };

            for (const auto& o : w["orbitals"]) {
                auto orbital = std::string{o["symbol"]};
                waypoint.orbitals.push_back(std::move(orbital));
            }

            system.waypoints.push_back(std::move(waypoint));
        }

        _systems.push_back(std::move(system));
    }

    auto factionsJson = _http(http::Request{
        .url = url / "factions",
        .headers = {authHeader()},
    }).json();

    for (const auto& faction : factionsJson["data"]) {
        _factions.push_back(Faction::json(faction));
    }
}

std::pair<std::string, std::string> World::authHeader() const
{
    return {"Authorization", std::format("Bearer {}", _token)};
}