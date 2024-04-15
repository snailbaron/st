#include "view.hpp"

View::View()
    : _window{
        "ST",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        1024,
        768,
        SDL_WINDOW_RESIZABLE}
    , _renderer{
        _window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC}
{ }

bool View::processInput()
{
    for (SDL_Event e; SDL_PollEvent(&e); ) {
        if (e.type == SDL_QUIT) {
            return false;
        }
    }

    return true;
}

void View::update(float delta)
{
    (void)delta;
}

void View::present()
{


    _renderer.present();
}
