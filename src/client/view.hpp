#pragma once

#include "widgets.hpp"
#include "world.hpp"
#include "resources.hpp"

#include <sdl.hpp>

class Camera {
public:
    Point<float> worldToScreen(const Point<float>& worldPoint) const;
    Point<float> screenToWorld(const Point<float>& screenPoint) const;

    void updateScreenSize(int w, int h);
    void focus(const Point<float>& worldPoint);
    void zoomIn(int amount);
    void move(int dx, int dy);

private:
    Point<float> worldUpLeft() const;
    float worldToScreenRatio() const;
    float screenToWorldRatio() const;

    int _zoomLevel = 1;
    int _screenWidth = 0;
    int _screenHeight = 0;
    Point<float> _worldCenter;
};

class View {
public:
    View(const World& world);
    ~View();

    bool processInput();
    void update(float delta);
    void present();

private:
    const World& _world;

    sdl::Window _window;
    sdl::Renderer _renderer;
    Camera _camera;
    UI _ui;

    bool _drag = false;
};