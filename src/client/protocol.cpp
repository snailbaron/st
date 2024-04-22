#include "protocol.hpp"

#include <error.hpp>

#include <algorithm>
#include <initializer_list>
#include <iterator>
#include <map>
#include <utility>

namespace {

template <class T>
    requires std::is_scoped_enum_v<T>
class Names {
public:
    explicit Names(std::initializer_list<std::pair<T, std::string_view>> l)
    {
        for (const auto& [value, name] : l) {
            {
                auto [it, inserted] = _valueByName.emplace(name, value);
                if (!inserted) {
                    throw e::Error{} <<
                        "duplicate name '" << name << "' for " << typeid(T).name();
                }
            }

            {
                auto [it, inserted] = _nameByValue.emplace(value, name);
                if (!inserted) {
                    throw e::Error{} <<
                        "duplicate value (" << std::to_underlying(value) <<
                        ") for " << typeid(T).name();
                }
            }
        }
    }

    T operator[](std::string_view name) const
    {
        if (auto it = _valueByName.find(std::string{name});
                it != _valueByName.end()) {
            return it->second;
        }
        throw e::Error{} <<
            "unknown name for " << typeid(T).name() << ": " << name;
    }

    const std::string& operator[](T value) const
    {
        return _nameByValue.at(value);
    }

    std::istream& read(std::istream& input, T& value) const
    {
        auto string = std::string{};
        input >> string;
        value = (*this)[string];
        return input;
    }

private:
    std::map<std::string, T> _valueByName;
    std::map<T, std::string> _nameByValue;
};

const auto waypointTypeNames = Names<WaypointType>{
    {WaypointType::Asteroid, "ASTEROID"},
    {WaypointType::FuelStation, "FUEL_STATION"},
    {WaypointType::GasGiant, "GAS_GIANT"},
    {WaypointType::JumpGate, "JUMP_GATE"},
    {WaypointType::Moon, "MOON"},
    {WaypointType::Planet, "PLANET"},
};

const auto systemTypeNames = Names<SystemType>{
    {SystemType::BlueStar, "BLUE_STAR"},
    {SystemType::OrangeStar, "ORANGE_STAR"},
    {SystemType::RedStar, "RED_STAR"},
    {SystemType::WhiteDwarf, "WHITE_DWARF"},
    {SystemType::YoungStar, "YOUNG_STAR"},
};

const auto traitSymbolNames = Names<TraitSymbol>{
    {TraitSymbol::Innovative, "INNOVATIVE"},
    {TraitSymbol::Bold, "BOLD"},
    {TraitSymbol::Visionary, "VISIONARY"},
    {TraitSymbol::Cooperative, "COOPERATIVE"},
    {TraitSymbol::Curious, "CURIOUS"},
    {TraitSymbol::United, "UNITED"},
    {TraitSymbol::Peaceful, "PEACEFUL"},
    {TraitSymbol::Strategic, "STRATEGIC"},
    {TraitSymbol::Intelligent, "INTELLIGENT"},
    {TraitSymbol::ResearchFocused, "RESEARCH_FOCUSED"},
    {TraitSymbol::Collaborative, "COLLABORATIVE"},
    {TraitSymbol::Progressive, "PROGRESSIVE"},
    {TraitSymbol::Militaristic, "MILITARISTIC"},
    {TraitSymbol::Aggressive, "AGGRESSIVE"},
    {TraitSymbol::Imperialistic, "IMPERIALISTIC"},
    {TraitSymbol::Industrious, "INDUSTRIOUS"},
    {TraitSymbol::Scavengers, "SCAVENGERS"},
    {TraitSymbol::TreasureHunters, "TREASURE_HUNTERS"},
    {TraitSymbol::Resourceful, "RESOURCEFUL"},
    {TraitSymbol::Dexterous, "DEXTEROUS"},
    {TraitSymbol::Unpredictable, "UNPREDICTABLE"},
    {TraitSymbol::Brutal, "BRUTAL"},
    {TraitSymbol::Fleeting, "FLEETING"},
    {TraitSymbol::Adaptable, "ADAPTABLE"},
    {TraitSymbol::Daring, "DARING"},
    {TraitSymbol::Exploratory, "EXPLORATORY"},
    {TraitSymbol::Flexible, "FLEXIBLE"},
    {TraitSymbol::Secretive, "SECRETIVE"},
    {TraitSymbol::Smugglers, "SMUGGLERS"},
    {TraitSymbol::Defensive, "DEFENSIVE"},
    {TraitSymbol::SelfSufficient, "SELF_SUFFICIENT"},
    {TraitSymbol::Proud, "PROUD"},
    {TraitSymbol::Welcoming, "WELCOMING"},
    {TraitSymbol::Diverse, "DIVERSE"},
};

template <class T, class InputRange>
requires std::ranges::input_range<InputRange> &&
    std::ranges::sized_range<InputRange>
std::vector<T> transformVector(
    InputRange&& source, const auto& op)
{
    auto result = std::vector<T>{};
    result.reserve(std::ranges::size(source));
    std::ranges::transform(source, std::back_inserter(result), op);
    return result;
}

} // namespace

SectorName::SectorName(std::string name)
    : _sectorName(std::move(name))
{
}

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

template <>
WaypointType fromString<WaypointType>(std::string_view string)
{
    return waypointTypeNames[string];
}

WaypointType waypointType(const std::string& string)
{
    return waypointTypeNames[string];
}

std::ostream& operator<<(std::ostream& output, const WaypointType& waypointType)
{
    return output << waypointTypeNames[waypointType];
}

std::istream& operator>>(std::istream& input, WaypointType& waypointType)
{
    return waypointTypeNames.read(input, waypointType);
}

SystemType systemType(const std::string& string)
{
    return systemTypeNames[string];
}

std::ostream& operator<<(std::ostream& output, const SystemType& systemType)
{
    return output << systemTypeNames[systemType];
}

std::istream& operator>>(std::istream& input, SystemType& systemType)
{
    return systemTypeNames.read(input, systemType);
}

template<>
TraitSymbol fromString<TraitSymbol>(std::string_view string)
{
    return traitSymbolNames[string];
}

Trait Trait::json(const nlohmann::json& j)
{
    return Trait{
        .symbol = fromString<TraitSymbol>(j["symbol"]),
        .name = j["name"],
        .description = j["description"],
    };
}

Faction Faction::json(const nlohmann::json& j)
{
    return Faction{
        .symbol = j["symbol"],
        .description = j["description"],
        .headquarters = j["headquarters"],
        .traits = j["traits"]
            | std::views::transform([] (const auto& jt) { return Trait::json(jt); })
            | std::ranges::to<std::vector<Trait>>(),
        //transformVector<Trait>(j["traits"], Trait::json),
        .isRecruiting = j["isRecruiting"],
    };
}

