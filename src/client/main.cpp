#include "timer.hpp"
#include "view.hpp"
#include "world.hpp"

#include <sdl.hpp>

#include <exception>
#include <iostream>

int main(int, char*[]) try
{
    auto sdlInit = sdl::Init{SDL_INIT_VIDEO | SDL_INIT_AUDIO};
    auto imgInit = img::Init{IMG_INIT_PNG};
    auto ttfInit = ttf::Init{};

    auto world = World{};
    auto view = View{world};

    auto timer = FrameTimer{240};
    for (;;) {
        if (!view.processInput()) {
            break;
        }

        if (auto framesPassed = timer()) {
            // TODO: update world by delta
            view.update(framesPassed * timer.delta());
            view.present();
        }

        timer.relax();
    }

    return 0;
} catch (...) {
    e::handleError();
    return EXIT_FAILURE;
}