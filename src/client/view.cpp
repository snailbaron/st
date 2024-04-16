#include "view.hpp"

#include <imgui_impl_sdl2.h>
#include <imgui_impl_sdlrenderer2.h>

#include <cmath>

namespace {

SDL_FRect pointRect(const Point& screenPoint)
{
    return SDL_FRect{
        .x = screenPoint.x - 5,
        .y = screenPoint.y - 5,
        .w = 10,
        .h = 10,
    };
}

} // namespace

Point Camera::worldToScreen(const Point& worldPoint) const
{
    return Point{
        .x = (worldPoint.x - worldUpLeft().x) * screenToWorldRatio(),
        .y = (worldUpLeft().y - worldPoint.y) * screenToWorldRatio(),
    };
}

Point Camera::screenToWorld(const Point& screenPoint) const
{
    return Point{
        .x = worldUpLeft().x + screenPoint.x * worldToScreenRatio(),
        .y = worldUpLeft().y - screenPoint.y * worldToScreenRatio(),
    };
}

void Camera::updateScreenSize(int w, int h)
{
    _screenWidth = w;
    _screenHeight = h;
}

void Camera::focus(const Point& worldPoint)
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

Point Camera::worldUpLeft() const
{
    return Point{
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
    , _imgui(ImGui::CreateContext())
{
    ImGui_ImplSDL2_InitForSDLRenderer(_window.ptr(), _renderer.ptr());
    ImGui_ImplSDLRenderer2_Init(_renderer.ptr());

    _camera.updateScreenSize(1024, 768);
    _camera.focus({0, 0});
}

View::~View()
{
    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext(_imgui);
}

bool View::processInput()
{
    for (SDL_Event e; SDL_PollEvent(&e); ) {
        if (e.type == SDL_QUIT) {
            return false;
        }

        ImGui_ImplSDL2_ProcessEvent(&e);

        if (!ImGui::GetIO().WantCaptureMouse) {
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


    }

    return true;
}

void View::update(float delta)
{
    (void)delta;
}

void View::present()
{
    ImGui_ImplSDLRenderer2_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

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

    //ImGui::ShowDemoWindow();

    ImGui::Render();
    ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());

    _renderer.present();
}

