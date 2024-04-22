#pragma once

#pragma once

#include "geometry.hpp"
#include "screen_coordinate.hpp"
#include "sdl.hpp"

#include <array>
#include <concepts>
#include <functional>
#include <memory>
#include <utility>
#include <vector>

#include <iostream>

class Widget {
public:
    enum class State {
        Idle,
        Hovered,
        Pressed,
        Lost,
    };

    Widget() {}
    virtual ~Widget() = 0 { }

    virtual Widget* locate(int x, int y)
    {
        if (intersect(Point{(float)x, (float)y}, _position)) {
            return this;
        }
        return nullptr;
    }

    float width() const;
    float height() const;

    virtual void act() const { }
    virtual void render(
        sdl::Renderer& renderer, const Vector<float>& offset = {}) = 0;
    virtual void update(float /*delta*/) {}

    const State& state() const { return _state; }

    void hover()
    {
        _state = stateTransitions[std::to_underlying(_state)][0];
        onHover();
    }

    void unhover()
    {
        _state = stateTransitions[std::to_underlying(_state)][1];
        onUnhover();
    }

    void press()
    {
        _state = stateTransitions[std::to_underlying(_state)][2];
        onPress();
    }

    void release()
    {
        _state = stateTransitions[std::to_underlying(_state)][3];
        onRelease();
    }

protected:
    virtual void onHover() {}
    virtual void onUnhover() {}
    virtual void onPress() {}
    virtual void onRelease() {}

    Rect<float> _position;

private:
    static constexpr auto stateTransitions = std::array{
        //         hover           unhover      press           release
        std::array{State::Hovered, State::Idle, State::Pressed, State::Idle}, // from idle
        std::array{State::Hovered, State::Idle, State::Pressed, State::Hovered}, // from hovered
        std::array{State::Pressed, State::Lost, State::Pressed, State::Hovered}, // from pressed
        std::array{State::Pressed, State::Lost, State::Lost, State::Idle}, // from lost
    };

    State _state = State::Idle;
};

class WidgetStorage {
public:
    virtual ~WidgetStorage() = default;

    template <std::derived_from<Widget> W, class... Args>
        requires std::constructible_from<W, Args...>
    W* add(Args&&... args)
    {
        auto ptr = std::make_unique<W>(std::forward<Args>(args)...);
        auto raw = ptr.get();
        _widgets.push_back(std::move(ptr));
        return raw;
    }

protected:
    std::vector<std::unique_ptr<Widget>> _widgets;
};

class UI : public WidgetStorage {
public:
    void render(sdl::Renderer& renderer) const;
    bool processEvent(const SDL_Event& event);
    void update(float delta);

private:
    Widget* widgetUnderCursor(int x, int y);

    Widget* _hovered = nullptr;
    Widget* _pressed = nullptr;
};

class Button : public Widget {
public:
    Button(sdl::Renderer& renderer);

    Button* position(float x, float y, float w, float h);
    Button* center(float x, float y);
    Button* size(float x, float y);
    Button* text(const char* text);
    Button* action(std::function<void()> action);

    void act() const override;
    void render(sdl::Renderer& renderer, const Vector<float>& offset = {}) override;

private:
    static constexpr auto outerColors = std::array{
        SDL_Color{100, 50, 50, 255}, // idle
        SDL_Color{120, 70, 70, 255}, // hovered
        SDL_Color{70, 120, 70, 255}, // pressed
        SDL_Color{50, 100, 50, 255}, // lost
    };
    static constexpr auto innerColors = std::array{
        SDL_Color{100, 100, 100, 255}, // idle
        SDL_Color{120, 120, 120, 255}, // hovered
        SDL_Color{120, 150, 120, 255}, // pressed
        SDL_Color{80, 100, 80, 255}, // lost
    };

    const SDL_Color& outerColor() const;
    const SDL_Color& innerColor() const;

    sdl::Renderer& _renderer;
    SDL_FRect _outerRect;
    SDL_FRect _innerRect;
    Vector<int> _textSize;
    std::function<void()> _action;
    sdl::Texture _textTexture;
};

class VerticalPanel : public Widget, public WidgetStorage {
public:
    VerticalPanel* position(float x, float y, float w, float h);

    void render(sdl::Renderer& renderer, const Vector<float>& offset = {}) override;

private:
    float _gap = 5.f;
};

class TextBox : public Widget {
public:
    TextBox(sdl::Renderer& renderer);

    TextBox* maxWidth(float w);
    TextBox* maxHeight(float h);
    TextBox* position(float x, float y, float w, float h);
    TextBox* text(std::string_view text);

    void render(sdl::Renderer& renderer, const Vector<float>& offset = {}) override;

private:
    static constexpr float _border = 2.f;
    static constexpr float _padding = 5.f;
    static constexpr float _scrollBarWidth = 3.f;

    sdl::Renderer& _renderer;
    std::string _text;
    float _maxWidth = 0.f;
    float _maxHeight = 0.f;
    float _testOffset = 0.f;
    bool _testForward = true;
};

class InfoBar : public Widget {
public:
    InfoBar();

    void render(sdl::Renderer& renderer, const Vector<float>& offset = {}) override;
};