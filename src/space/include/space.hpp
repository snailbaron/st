#pragma once

#include "http.hpp"

#include <optional>
#include <ostream>
#include <string>
#include <string_view>

namespace space {

struct System {
    std::string fullName() const;

    std::string sector;
    std::string system;
};

std::ostream& operator<<(std::ostream& output, const System& system);
std::optional<System> parseSystem(const std::string& string);

struct Waypoint {
    std::string fullSystemName() const;
    std::string fullName() const;

    std::string sector;
    std::string system;
    std::string point;
};

std::ostream& operator<<(std::ostream& output, const Waypoint& waypoint);
std::optional<Waypoint> parseWaypoint(const std::string& string);

class API {
public:
    [[nodiscard]] std::string registerNewAgent(
        std::string_view symbol, std::string_view faction = "COSMIC");

    void agentInfo();

    void info(const Waypoint& waypoint);
    void info(const System& system);

private:
    http::Init _httpInit;
    http::Session _session;
};

} // namespace