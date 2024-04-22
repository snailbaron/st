#include "resources.hpp"

#include <fs.hpp>

namespace {

constexpr auto fontSize = 14;

} // namespace

void Resources::load(sdl::Renderer& renderer)
{
    _renderer = &renderer;
    auto fonts = fs::exeDir() / "assets" / "fonts";

    _fontData.emplace(
        Font::Furore,
        fs::readBytes(fonts / "Furore" / "Furore.otf"));
    _fontData.emplace(
        Font::Orbitron,
        fs::readBytes(fonts / "Orbitron" / "orbitron-medium.otf"));

    // TODO: load images
    (void)renderer;
}

void Resources::clear()
{
    _fonts.clear();
}

ttf::Font& Resources::operator()(Font f, int pt)
{
    auto key = FontAndSize{.font = f, .size = pt};

    if (auto it = _fonts.find(key); it != _fonts.end()) {
        return it->second;
    }

    auto [it, inserted] = _fonts.emplace(key, ttf::Font{_fontData.at(f), pt});
    it->second.setHinting(TTF_HINTING_LIGHT_SUBPIXEL);
    return it->second;
}