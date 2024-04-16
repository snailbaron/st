#include "world.hpp"

#include <fs.hpp>

#include <format>
#include <utility>
#include <vector>

namespace {

const auto url = http::URL{"https://api.spacetraders.io/v2"};

const auto waypointTypeNames = std::vector<std::pair<WaypointType, std::string_view>>{
    {WaypointType::Asteroid, "ASTEROID"},
    {WaypointType::FuelStation, "FUEL_STATION"},
    {WaypointType::GasGiant, "GAS_GIANT"},
    {WaypointType::JumpGate, "JUMP_GATE"},
    {WaypointType::Moon, "MOON"},
    {WaypointType::Planet, "PLANET"},
};

const auto systemTypeNames = std::vector<std::pair<SystemType, std::string_view>>{
    {SystemType::BlueStar, "BLUE_STAR"},
    {SystemType::OrangeStar, "ORANGE_STAR"},
    {SystemType::RedStar, "RED_STAR"},
    {SystemType::WhiteDwarf, "WHITE_DWARF"},
    {SystemType::YoungStar, "YOUNG_STAR"},
};

auto valueByName(const auto& mapping, std::string_view name)
{
    auto it = std::ranges::find(
        mapping,
        name,
        [] (const auto& pair) {
            return pair.second;
        });
    if (it == mapping.end()) {
        throw e::Error{} << "could not find name: " << name;
    }
    return it->first;
}

std::istream& valueFromStream(const auto& mapping, std::istream& input, auto& value)
{
    auto name = std::string{};
    input >> name;
    value = valueByName(mapping, name);
    return input;
}

std::string_view nameByValue(const auto& mapping, const auto& value)
{
    auto it = std::ranges::find(
        mapping,
        value,
        [] (const auto& pair) {
            return pair.first;
        });
    e::require(it != mapping.end(), "could not find value");
    return it->second;
}

} // namespace

SectorName::SectorName(std::string name)
    : _sectorName(std::move(name))
{ }

std::ostream& operator<<(std::ostream& output, const SectorName& sector)
{
    return output << sector._sectorName;
}

SystemName::SystemName(std::string_view name)
{
    auto sep = name.find('-');
    e::require(sep != std::string_view::npos);
    _sector = SectorName{std::string{name.substr(0, sep)}};
    _systemPart = name.substr(sep + 1);
}

std::ostream& operator<<(std::ostream& output, const SystemName& system)
{
    return output << system._sector << "-" << system._systemPart;
}

WaypointName::WaypointName(std::string_view name)
{
    auto sep = name.find('-');
    e::require(sep != std::string_view::npos);
    sep = name.find('-', sep + 1);
    e::require(sep != std::string_view::npos);

    _system = SystemName{name.substr(0, sep)};
    _waypointPart = name.substr(sep + 1);
}

std::ostream& operator<<(std::ostream& output, const WaypointName& waypoint)
{
    return output << waypoint._system << "-" << waypoint._waypointPart;
}

WaypointType waypointType(const std::string& string)
{
    return valueByName(waypointTypeNames, string);
}

std::ostream& operator<<(std::ostream& output, const WaypointType& waypointType)
{
    return output << nameByValue(waypointTypeNames, waypointType);
}

std::istream& operator>>(std::istream& input, WaypointType& waypointType)
{
    return valueFromStream(waypointTypeNames, input, waypointType);
}

SystemType systemType(const std::string& string)
{
    return valueByName(systemTypeNames, string);
}

std::ostream& operator<<(std::ostream& output, const SystemType& systemType)
{
    return output << nameByValue(systemTypeNames, systemType);
}

std::istream& operator>>(std::istream& input, SystemType& systemType)
{
    return valueFromStream(systemTypeNames, input, systemType);
}

World::World()
    : _token(fs::read(fs::home() / ".space_traders_token"))
{
    //auto agentData = _http(http::Request{
    //    .url = url / "my/agent",
    //    .headers = {authHeader()},
    //});

    //auto json = agentData.json();
    //auto headquarters = Waypoint{json["headquarters"]};

    auto json = _http(http::Request{
        .url = url / "systems",
        .headers = {authHeader()},
    }).json();

    const auto& systems = json["data"];
    for (const auto& s : systems) {
        auto system = System{
            .symbol = s["symbol"],
            .sectorSymbol = s["sectorSymbol"],
            .type = systemType(s["type"]),
            .point = Point{s["x"], s["y"]},
            .waypoints = {},
        };

        for (const auto& w : s["waypoints"]) {
            auto waypoint = Waypoint{
                .symbol = w["symbol"],
                .type = waypointType(w["type"]),
                .point = Point{w["x"], w["y"]},
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
}

std::pair<std::string, std::string> World::authHeader() const
{
    return {"Authorization", std::format("Bearer {}", _token)};
}