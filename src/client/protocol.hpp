#pragma once

#include "geometry.hpp"

#include <error.hpp>

#include <nlohmann/json.hpp>

#include <string_view>
#include <string>
#include <type_traits>
#include <vector>

template <class T>
requires std::is_scoped_enum_v<T>
T fromString(std::string_view string);

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

extern template WaypointType fromString<WaypointType>(std::string_view string);
std::ostream& operator<<(std::ostream& output, const WaypointType& waypointType);
std::istream& operator>>(std::istream& input, WaypointType& waypointType);

struct Waypoint {
    std::string symbol;
    WaypointType type;
    Point<float> point;
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
    Point<float> point;
    std::vector<Waypoint> waypoints;
};

enum class TestEnum {
    One,
    Two,
    ThreeFour,
};

template <>
inline TestEnum fromString<TestEnum>(std::string_view string)
{
    if (string == "ONE") return TestEnum::One;
    if (string == "TWO") return TestEnum::Two;
    if (string == "THREE_FOUR") return TestEnum::ThreeFour;
    throw e::Error{} << "unknown name (" << string << ") of " << "TestEnum";
}

inline std::istream& operator>>(std::istream& input, TestEnum& value)
{
    auto string = std::string{};
    input >> string;
    value = fromString<TestEnum>(string);
    return input;
}

inline std::ostream& operator<<(std::ostream& output, const TestEnum& value)
{
    switch (value) {
        case TestEnum::One: return output << "ONE";
        case TestEnum::Two: return output << "TWO";
        case TestEnum::ThreeFour: return output << "THREE_FOUR";
    }
    throw e::Error{} <<
        "unknown value (" << std::to_underlying(value) << ") of " << "TestEnum";
}

enum class TraitSymbol {
    Innovative,
    Bold,
    Visionary,
    Cooperative,
    Curious,
    United,
    Peaceful,
    Strategic,
    Intelligent,
    ResearchFocused,
    Collaborative,
    Progressive,
    Militaristic,
    Aggressive,
    Imperialistic,
    Industrious,
    Scavengers,
    TreasureHunters,
    Resourceful,
    Dexterous,
    Unpredictable,
    Brutal,
    Fleeting,
    Adaptable,
    Daring,
    Exploratory,
    Flexible,
    Secretive,
    Smugglers,
    Defensive,
    SelfSufficient,
    Proud,
    Welcoming,
    Diverse,
};

extern template TraitSymbol fromString<TraitSymbol>(std::string_view string);

struct Trait {
    static Trait json(const nlohmann::json& j);

    TraitSymbol symbol;
    std::string name;
    std::string description;
};

struct Faction {
    static Faction json(const nlohmann::json& j);

    std::string symbol;
    std::string name;
    std::string description;
    std::string headquarters;
    std::vector<Trait> traits;
    bool isRecruiting = false;
};
