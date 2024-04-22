#pragma once

#include "geometry.hpp"
#include "protocol.hpp"

#include <http.hpp>

#include <istream>
#include <ostream>
#include <string>
#include <string_view>

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
    std::vector<Faction> _factions;
};