#pragma once

#include <sdl.hpp>

class View {
public:
    View();

    bool processInput();
    void update(float delta);
    void present();

private:
    sdl::Window _window;
    sdl::Renderer _renderer;
};