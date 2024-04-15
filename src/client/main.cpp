#include "timer.hpp"
#include "view.hpp"

#include <sdl.hpp>

#include <iostream>

int main(int, char*[]) try
{
    auto sdlInit = sdl::Init{SDL_INIT_VIDEO | SDL_INIT_AUDIO};
    auto imgInit = img::Init{IMG_INIT_PNG};
    auto ttfInit = ttf::Init{};

    auto view = View{};

    auto timer = FrameTimer{60};
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
} catch (const sdl::Error& e) {
    std::cerr << e.what() << "\n";
    return EXIT_FAILURE;
}