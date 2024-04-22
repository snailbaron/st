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

SDL_FRect shift(const SDL_FRect& rect, const Vector<float>& shift)
{
    return SDL_FRect{
        rect.x + shift.x,
        rect.y + shift.y,
        rect.w,
        rect.h,
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

Widget* Button::locate(int x, int y)
{
    if (_position.contains({(float)x, (float)y})) {
        return this;
    }
    return nullptr;
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

FlexTextBox::FlexTextBox(sdl::Renderer& renderer)
    : _renderer(&renderer)
{ }

FlexTextBox* FlexTextBox::renderer(sdl::Renderer& renderer)
{
    _renderer = &renderer;
    return this;
}

FlexTextBox* FlexTextBox::maxWidth(uint32_t w)
{
    _maxWidth = w;
    return this;
}

FlexTextBox* FlexTextBox::position(float x, float y, float w, float h)
{
    _position = {x, y, w, h};
    return this;
}

FlexTextBox* FlexTextBox::text(std::string_view text)
{
    _text = text;

    auto& font = resources(Font::Orbitron, 12);
    auto textSurface = font.renderUtf8BlendedWrapped(
        _text, SDL_Color{0, 0, 0, 255}, _maxWidth);
    _outerRect = SDL_FRect{
        _position.corner().x,
        _position.corner().y,
        (float)textSurface->w + 2 * _border + 2 * _padding,
        (float)textSurface->h + 2 * _border + 2 * _padding,
    };
    _innerRect = SDL_FRect{
        _outerRect.x + _border,
        _outerRect.y + _border,
        _outerRect.w - 2 * _border,
        _outerRect.h - 2 * _border,
    };
    _textRect = SDL_FRect{
        _innerRect.x + _padding,
        _innerRect.y + _padding,
        (float)textSurface->w,
        (float)textSurface->h,
    };

    _textTexture = _renderer->createTextureFromSurface(textSurface);

    return this;
}

void FlexTextBox::render(sdl::Renderer& renderer, const Vector<float>& offset)
{
    renderer.setDrawColor(0, 0, 0, 255);
    renderer.fillRect(shift(_outerRect, offset));

    renderer.setDrawColor(170, 150, 150, 255);
    renderer.fillRect(shift(_innerRect, offset));

    renderer.copy(_textTexture, shift(_textRect, offset));
}

TextWithPopup::TextWithPopup(sdl::Renderer& renderer)
    : _renderer(&renderer)
    , _popup(renderer)
{ }

TextWithPopup* TextWithPopup::position(float x, float y)
{
    _position = {x, y, _position.width(), _position.height()};
    return this;
}

TextWithPopup* TextWithPopup::text(const std::string& text)
{
    auto& font = resources(Font::Orbitron, 14);

    auto normalSurface = font.renderUtf8Blended(text, SDL_Color{0, 0, 0, 255});
    auto hoverSurface = font.renderUtf8Blended(text, SDL_Color{180, 0, 0, 255});
    _position = {_position.minX(), _position.minY(), (float)normalSurface->w, (float)normalSurface->h};

    _normalTexture = _renderer->createTextureFromSurface(normalSurface);
    _hoverTexture = _renderer->createTextureFromSurface(hoverSurface);

    return this;
}

TextWithPopup* TextWithPopup::popup(std::string popup)
{
    _popup.text(popup);
    return this;
}

Widget* TextWithPopup::locate(int x, int y)
{
    if (_position.contains({(float)x, (float)y})) {
        return this;
    }
    return nullptr;
}

void TextWithPopup::render(sdl::Renderer& renderer, const Vector<float>& offset)
{
    if (_state == State::Hovered || _state == State::Pressed) {
        renderer.copy(_hoverTexture, toSdl(_position + offset));
        _popup.render(renderer, offset + _position.corner().vector() + Vector<float>{0.f, _position.height() + 2.f});
    } else {
        renderer.copy(_normalTexture, toSdl(_position + offset));
    }
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

Box* Box::geometry(float x, float y, float w, float h)
{
    _position = {x, y, w, h};
    return this;
}

void Box::render(sdl::Renderer& renderer, const Vector<float>& offset)
{
    auto outerRect = _position + offset;
    auto innerRect = outerRect.shrinked(2.f);

    renderer.setDrawColor(0, 0, 0, 255);
    renderer.fillRect(toSdl(outerRect));

    renderer.setDrawColor(180, 150, 150, 255);
    renderer.fillRect(toSdl(innerRect));
}
