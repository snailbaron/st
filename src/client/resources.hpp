#pragma once

#include <sdl.hpp>

#include <map>
#include <vector>

enum class Font {
    Furore,
    Orbitron,
};

class Resources {
public:
    void load(sdl::Renderer& renderer);
    void clear();

    ttf::Font& operator()(Font f, int pt);

private:
    struct FontAndSize {
        Font font;
        int size = 0;

        auto operator<=>(const FontAndSize&) const = default;
    };

    const sdl::Renderer* _renderer = nullptr;
    std::map<Font, std::vector<std::byte>> _fontData;
    std::map<FontAndSize, ttf::Font> _fonts;
};

inline Resources resources;