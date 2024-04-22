#include "view.hpp"

#include <cmath>

namespace {

SDL_FRect pointRect(const Point<float>& screenPoint)
{
    return SDL_FRect{
        .x = screenPoint.x - 5,
        .y = screenPoint.y - 5,
        .w = 10,
        .h = 10,
    };
}

} // namespace

Point<float> Camera::worldToScreen(const Point<float>& worldPoint) const
{
    return Point{
        .x = (worldPoint.x - worldUpLeft().x) * screenToWorldRatio(),
        .y = (worldUpLeft().y - worldPoint.y) * screenToWorldRatio(),
    };
}

Point<float> Camera::screenToWorld(const Point<float>& screenPoint) const
{
    return Point<float>{
        .x = worldUpLeft().x + screenPoint.x * worldToScreenRatio(),
        .y = worldUpLeft().y - screenPoint.y * worldToScreenRatio(),
    };
}

void Camera::updateScreenSize(int w, int h)
{
    _screenWidth = w;
    _screenHeight = h;
}

void Camera::focus(const Point<float>& worldPoint)
{
    _worldCenter = worldPoint;
}

void Camera::zoomIn(int amount)
{
    _zoomLevel -= amount;
    std::cerr << "zoom level = " << _zoomLevel << "\n";
}

void Camera::move(int dx, int dy)
{
    _worldCenter.x -= dx * worldToScreenRatio();
    _worldCenter.y += dy * worldToScreenRatio();
}

Point<float> Camera::worldUpLeft() const
{
    return Point<float>{
        .x = _worldCenter.x - (_screenWidth * 0.5f) * worldToScreenRatio(),
        .y = _worldCenter.y + (_screenHeight * 0.5f) * worldToScreenRatio(),
    };
}

float Camera::worldToScreenRatio() const
{
    return std::pow(1.2f, (float)_zoomLevel);
}

float Camera::screenToWorldRatio() const
{
    return 1.f / worldToScreenRatio();
}

View::View(const World& world)
    : _world(world)
    , _window{
        "ST",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        1024,
        768,
        SDL_WINDOW_RESIZABLE}
    , _renderer{
        _window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC}
{
    resources.load(_renderer);

    _camera.updateScreenSize(1024, 768);
    _camera.focus({0, 0});

    _ui.add<Button>(_renderer)
        ->position(10, 10, 100, 30)
        ->text("Contracts")
        ->action([] {
            std::cerr << "contracts pressed\n";
        });
    _ui.add<Button>(_renderer)
        ->position(10, 45, 100, 30)
        ->text("Factions")
        ->action([] {
            std::cerr << "factions pressed\n";
        });

    _ui.add<TextBox>(_renderer)
        ->position(300, 100, 200, 200)
        ->maxWidth(700)
        ->maxHeight(100)
        ->text(
            "The Cosmic Engineers are a group of highly advanced scientists and "
            "engineers who seek to terraform and colonize new worlds, pushing the "
            "boundaries of technology and exploration."
        );
}

View::~View()
{
    resources.clear();
}

bool View::processInput()
{
    for (SDL_Event e; SDL_PollEvent(&e); ) {
        if (e.type == SDL_QUIT) {
            return false;
        }

        if (_ui.processEvent(e)) {
            continue;
        }

        if (e.type == SDL_MOUSEBUTTONDOWN &&
                e.button.button == SDL_BUTTON_LEFT) {
            _drag = true;
        } else if (e.type == SDL_MOUSEBUTTONUP &&
                e.button.button == SDL_BUTTON_LEFT) {
            _drag = false;
        } else if (_drag && e.type == SDL_MOUSEMOTION) {
            _camera.move(e.motion.xrel, e.motion.yrel);
        } else if (e.type == SDL_MOUSEWHEEL) {
            _camera.zoomIn(e.wheel.y);
        } else if (e.type == SDL_WINDOWEVENT &&
                e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
            std::cerr << "size changed: " << e.window.data1 << " x " << e.window.data2 << "\n";
            _camera.updateScreenSize(e.window.data1, e.window.data2);
        } else if (e.type == SDL_WINDOWEVENT &&
                e.window.event == SDL_WINDOWEVENT_RESIZED) {
            std::cerr << "resized: " << e.window.data1 << " x " << e.window.data2 << "\n";
        }
    }

    return true;
}

void View::update(float delta)
{
    _ui.update(delta);
}

void View::present()
{
    _renderer.setDrawColor(30, 30, 30, 255);
    _renderer.clear();

    for (const auto& system : _world.systems()) {
        _renderer.setDrawColor(150, 180, 180, 255);
        auto p = _camera.worldToScreen(system.point);
        _renderer.fillRect(pointRect(p));

        _renderer.setDrawColor(200, 150, 150, 255);
        for (const auto& waypoint : system.waypoints) {
            auto wp = _camera.worldToScreen(waypoint.point);
            _renderer.fillRect(pointRect(wp));
        }
    }

    _ui.render(_renderer);

    _renderer.present();
}

