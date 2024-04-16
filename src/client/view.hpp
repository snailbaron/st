#pragma once

#include "world.hpp"

#include <imgui.h>

#include <sdl.hpp>

class Camera {
public:
    Point worldToScreen(const Point& worldPoint) const;
    Point screenToWorld(const Point& screenPoint) const;

    void updateScreenSize(int w, int h);
    void focus(const Point& worldPoint);
    void zoomIn(int amount);
    void move(int dx, int dy);

private:
    Point worldUpLeft() const;
    float worldToScreenRatio() const;
    float screenToWorldRatio() const;

    int _zoomLevel = 1;
    int _screenWidth = 0;
    int _screenHeight = 0;
    Point _worldCenter;
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
    ImGuiContext* _imgui = nullptr;

    Camera _camera;

    bool _drag = false;
};