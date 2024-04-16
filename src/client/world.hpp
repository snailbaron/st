#pragma once

#include "geometry.hpp"

#include <http.hpp>

#include <istream>
#include <ostream>
#include <string>
#include <string_view>

class SectorName {
public:
    explicit SectorName(std::string name = "");

    friend std::ostream& operator<<(std::ostream& output, const SectorName& sector);

private:
    std::string _sectorName;
};

class SystemName {
public:
    explicit SystemName(std::string_view name = "");

    friend std::ostream& operator<<(std::ostream& output, const SystemName& system);

private:
    SectorName _sector;
    std::string _systemPart;
};

class WaypointName {
public:
    explicit WaypointName(std::string_view name = "");

    friend std::ostream& operator<<(std::ostream& output, const WaypointName& waypoint);

private:
    SystemName _system;
    std::string _waypointPart;
};

enum class WaypointType {
    Asteroid,
    FuelStation,
    GasGiant,
    JumpGate,
    Moon,
    Planet,
};

WaypointType waypointType(const std::string& string);
std::ostream& operator<<(std::ostream& output, const WaypointType& waypointType);
std::istream& operator>>(std::istream& input, WaypointType& waypointType);

struct Waypoint {
    std::string symbol;
    WaypointType type;
    Point point;
    std::vector<std::string> orbitals;
};

enum class SystemType {
    BlueStar,
    OrangeStar,
    RedStar,
    WhiteDwarf,
    YoungStar,
};

SystemType systemType(const std::string& string);
std::ostream& operator<<(std::ostream& output, const SystemType& systemType);
std::istream& operator>>(std::istream& input, SystemType& systemType);

struct System {
    std::string symbol;
    std::string sectorSymbol;
    SystemType type;
    Point point;
    std::vector<Waypoint> waypoints;
};

class World {
public:
    World();

    const std::vector<System>& systems() const
    {
        return _systems;
    }

private:
    std::pair<std::string, std::string> authHeader() const;

    std::string _token;
    http::Session _http;

    std::vector<System> _systems;
};