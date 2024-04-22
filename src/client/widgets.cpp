#include "widgets.hpp"

#include "resources.hpp"

namespace {

SDL_FRect toSdl(const Rect<float>& rect)
{
    return SDL_FRect{
        .x = rect.minX(),
        .y = rect.minY(),
        .w = rect.width(),
        .h = rect.height(),
    };
}

} // namespace

float Widget::width() const
{
    return _position.width();
}

float Widget::height() const
{
    return _position.height();
}

void UI::render(sdl::Renderer& renderer) const
{
    for (const auto& widget : _widgets) {
        widget->render(renderer);
    }
}

bool UI::processEvent(const SDL_Event& event)
{
    if (event.type == SDL_MOUSEBUTTONDOWN &&
        event.button.button == SDL_BUTTON_LEFT) {
        if (_hovered) {
            _pressed = _hovered;
            _pressed->press();
            return true;
        }
    } else if (event.type == SDL_MOUSEBUTTONUP &&
        event.button.button == SDL_BUTTON_LEFT) {
        if (_pressed) {
            _pressed->release();
            if (_hovered == _pressed) {
                _hovered->act();
            } else if (_hovered) {
                _hovered->hover();
            }
            _pressed = nullptr;
            return true;
        }
    } else if (event.type == SDL_MOUSEMOTION) {
        auto x = event.motion.x;
        auto y = event.motion.y;
        auto widget = widgetUnderCursor(x, y);

        if (_hovered && widget != _hovered) {
            _hovered->unhover();
            _hovered = nullptr;
        }

        if (!_hovered) {
            _hovered = widget;
            if (_hovered && (!_pressed || _hovered == _pressed)) {
                _hovered->hover();
            }
        }
    }

    return false;
}

void UI::update(float delta)
{
    for (const auto& widget : _widgets) {
        widget->update(delta);
    }
}

Widget* UI::widgetUnderCursor(int x, int y)
{
    for (const auto& widget : _widgets) {
        if (auto w = widget->locate(x, y)) {
            return w;
        }
    }
    return nullptr;
}

Button::Button(sdl::Renderer& renderer)
    : _renderer(renderer)
{ }

Button* Button::position(float x, float y, float w, float h)
{
    _position = {x, y, w, h};
    return this;
}

Button* Button::center(float x, float y)
{
    _position.center({x, y});
    return this;
}

Button* Button::size(float x, float y)
{
    _position.size({x, y});
    return this;
}

Button* Button::text(const char* text)
{
    auto textSurface =
        resources(Font::Furore, 14).renderUtf8Blended(text, SDL_Color{0, 0, 0, 255});
    _textSize = {textSurface->w, textSurface->h};
    _textTexture = _renderer.createTextureFromSurface(textSurface);
    return this;
}

Button* Button::action(std::function<void()> action)
{
    _action = std::move(action);
    return this;
}

void Button::act() const
{
    _action();
}

void Button::render(sdl::Renderer& renderer, const Vector<float>& offset)
{
    auto outerRect = _position + offset;
    auto innerRect = _position.shrinked(2.f);
    auto textRect = Rect<float>::fromCenter(outerRect.center(), _textSize);

    renderer.setDrawColor(outerColor());
    renderer.fillRect(toSdl(outerRect));

    renderer.setDrawColor(innerColor());
    renderer.fillRect(toSdl(innerRect));

    renderer.copy(_textTexture, toSdl(textRect));
}

const SDL_Color& Button::outerColor() const
{
    return outerColors[std::to_underlying(state())];
}

const SDL_Color& Button::innerColor() const
{
    return innerColors[std::to_underlying(state())];
}

VerticalPanel* VerticalPanel::position(float x, float y, float w, float h)
{
    _position = Rect<float>{x, y, w, h};
    return this;
}

void VerticalPanel::render(sdl::Renderer& renderer, const Vector<float>& offset)
{
    auto outerRect = _position + offset;
    auto innerRect = outerRect.shrinked(2.f);

    renderer.setDrawColor(0, 0, 0, 255);
    renderer.fillRect(toSdl(outerRect));

    renderer.setDrawColor(150, 170, 150, 255);
    renderer.fillRect(toSdl(innerRect));

}

TextBox::TextBox(sdl::Renderer& renderer)
    : _renderer(renderer)
{ }

TextBox* TextBox::maxWidth(float w)
{
    _maxWidth = w;
    return this;
}

TextBox* TextBox::maxHeight(float h)
{
    _maxHeight = h;
    return this;
}

TextBox* TextBox::position(float x, float y, float w, float h)
{
    _position = {x, y, w, h};
    return this;
}

TextBox* TextBox::text(std::string_view text)
{
    _text = text;
    return this;
}

void TextBox::render(sdl::Renderer& renderer, const Vector<float>& offset)
{
    auto& font = resources(Font::Orbitron, 14);

    auto ts = font.renderUtf8BlendedWrapped(_text, SDL_Color{0, 0, 0, 255}, 693);
    auto tt = _renderer.createTextureFromSurface(ts);
    _renderer.setDrawColor(SDL_Color{170, 150, 150, 255});
    _renderer.fillRect(SDL_FRect{25, 375, 800, 100});
    _renderer.copy(tt, SDL_FRect{53.0f, 400, (float)ts->w, (float)ts->h});


    auto maxTextWidth = static_cast<uint32_t>(_maxWidth - _border - _padding);
    auto maxTextHeight = static_cast<uint32_t>(_maxHeight - _border - _padding);

    bool textFits = true;
    auto textColor = SDL_Color{0, 0, 0, 255};
    auto bgColor = SDL_Color{170, 150, 150, 255};
    auto textSurface =
        font.renderUtf8BlendedWrapped(_text, textColor, maxTextWidth);
    auto srcrect = SDL_Rect{0, 0, textSurface->w, textSurface->h};
    if ((uint32_t)textSurface->h > maxTextHeight) {
        maxTextWidth -= static_cast<uint32_t>(_scrollBarWidth + 2 * _padding);
        textSurface =
            font.renderUtf8BlendedWrapped(_text, textColor, maxTextWidth);
        textFits = false;
        srcrect.h = maxTextHeight;
    }
    auto textTexture = _renderer.createTextureFromSurface(textSurface);

    auto boxWidth = textSurface->w + 2 * _padding + 2 * _border;
    if (!textFits) {
        boxWidth += _scrollBarWidth + _padding;
    }
    auto boxHeight =
        std::min(_maxHeight, textSurface->h + 2 * _padding + 2 * _border);

    auto outerRect = Rect<float>::fromCenter(_position.center() + offset, Vector<float>{boxWidth, boxHeight});
    auto innerRect = outerRect.shrinked(_border);
    auto textRect = innerRect.shrinked(_padding);
    if (!textFits) {
        textRect = Rect<float>{
            textRect.minX(),
            textRect.minY(),
            textRect.width() - _scrollBarWidth - _padding,
            textRect.height()};
    }

    renderer.setDrawColor(0, 0, 0, 255);
    renderer.fillRect(toSdl(outerRect));

    renderer.setDrawColor(170, 150, 150, 255);
    renderer.fillRect(toSdl(innerRect));

    auto badRect = toSdl(textRect);
    renderer.copy(textTexture, badRect);
}

InfoBar::InfoBar()
{
    //auto nameText = "Cosmic Engineers (COSMIC)";
    //auto descriptionText =
    //    "The Cosmic Engineers are a group of highly advanced scientists and "
    //    "engineers who seek to terraform and colonize new worlds, pushing the "
    //    "boundaries of technology and exploration.";
    //auto headquartersText = "Headquarters: X1-GA69";
    //auto recruitingText = "Recruiting";
}

void InfoBar::render(sdl::Renderer& renderer, const Vector<float>& offset)
{
    auto outerRect = _position + offset;
    auto innerRect = outerRect.shrinked(2.f);

    renderer.setDrawColor(0, 0, 0, 255);
    renderer.fillRect(toSdl(outerRect));

    renderer.setDrawColor(170, 150, 150, 255);
    renderer.fillRect(toSdl(innerRect));


}

