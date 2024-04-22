#pragma once

#include <cstddef>
#include <cstdint>
#include <exception>
#include <filesystem>
#include <memory>
#include <source_location>
#include <span>
#include <string>

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

namespace sdl {

class Error : public std::exception {
public:
    Error(std::source_location sl = std::source_location::current());
    const char* what() const noexcept override;

private:
    std::string _message;
};

inline void check(
    int returnCode, std::source_location sl = std::source_location::current())
{
    if (returnCode != 0) {
        throw Error{sl};
    }
}

template <class T>
T* check(T* ptr, std::source_location sl = std::source_location::current())
{
    if (ptr == nullptr) {
        throw Error{sl};
    }
    return ptr;
}

class Init {
public:
    Init(uint32_t flags);
    ~Init();

    Init(const Init&) = delete;
    Init(Init&&) = delete;
    Init& operator=(const Init&) = delete;
    Init& operator=(Init&&) = delete;
};

class RWops {
public:
    RWops() = default;
    explicit RWops(SDL_RWops* ptr);
    RWops(const std::filesystem::path& file, const char* mode);
    RWops(std::span<const std::byte> mem);

    SDL_RWops* ptr();
    const SDL_RWops* ptr() const;

private:
    std::unique_ptr<SDL_RWops, int(*)(SDL_RWops*)> _ptr{
        nullptr, SDL_RWclose};
};

class Surface {
public:
    Surface() = default;
    explicit Surface(SDL_Surface* ptr);

    SDL_Surface* ptr();
    const SDL_Surface* ptr() const;

    SDL_Surface& operator*();
    const SDL_Surface& operator*() const;

    SDL_Surface* operator->();
    const SDL_Surface* operator->() const;

private:
    std::unique_ptr<SDL_Surface, void(*)(SDL_Surface*)> _ptr{
        nullptr, SDL_FreeSurface};
};

class Texture {
public:
    Texture() = default;
    explicit Texture(SDL_Texture* ptr);

    SDL_Texture* ptr();
    const SDL_Texture* ptr() const;

private:
    std::unique_ptr<SDL_Texture, void(*)(SDL_Texture*)> _ptr{
        nullptr, SDL_DestroyTexture};
};

class Window {
public:
    Window() = default;
    Window(const char* title, int x, int y, int w, int h, uint32_t flags);

    SDL_Window* ptr();
    const SDL_Window* ptr() const;

private:
    std::unique_ptr<SDL_Window, void(*)(SDL_Window*)> _ptr{
        nullptr, SDL_DestroyWindow};
};

class Renderer {
public:
    Renderer() = default;
    Renderer(Window& window, int index, uint32_t flags);

    SDL_Renderer* ptr();
    const SDL_Renderer* ptr() const;

    Texture createTextureFromSurface(Surface& surface);
    Texture createTextureFromSurface(Surface&& surface);
    Texture loadTexture(const std::filesystem::path& file);
    Texture loadTexture(std::span<const std::byte> mem);

    void clear();
    void present();

    void setDrawColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
    void setDrawColor(const SDL_Color& color);

    void copy(
        Texture& texture, const SDL_Rect* srcrect, const SDL_Rect* dstrect);
    void copy(
        Texture& texture, const SDL_Rect* srcrect, const SDL_FRect* dstrect);
    void copy(Texture& texture, const SDL_Rect& srcrect, const SDL_FRect& dstrect);
    void copy(Texture& texture, const SDL_FRect& dstrect);

    void drawPoint(int x, int y);
    void drawPoint(float x, float y);
    void drawPoints(std::span<const SDL_Point> points);
    void drawPoints(std::span<const SDL_FPoint> points);
    void drawLine(int x1, int y1, int x2, int y2);
    void drawLine(float x1, float y1, float x2, float y2);
    void drawLines(std::span<const SDL_Point> points);
    void drawLines(std::span<const SDL_FPoint> points);
    void drawRect(const SDL_Rect& rect);
    void drawRect(const SDL_FRect& rect);
    void drawRects(std::span<const SDL_Rect> rects);
    void drawRects(std::span<const SDL_FRect> rects);

    void fillRect(const SDL_Rect& rect);
    void fillRect(const SDL_FRect& rect);
    void fillRects(std::span<const SDL_Rect> rects);
    void fillRects(std::span<const SDL_FRect> rects);

    void renderGeometry(std::span<const SDL_Vertex> vertices);
    void renderGeometry(Texture& texture, std::span<const SDL_Vertex> vertices);
    void renderGeometry(
        std::span<const SDL_Vertex> vertices, std::span<const int> indices);
    void renderGeometry(
        Texture& texture,
        std::span<const SDL_Vertex> vertices,
        std::span<const int> indices);

private:
    std::unique_ptr<SDL_Renderer, void(*)(SDL_Renderer*)> _ptr{
        nullptr, SDL_DestroyRenderer};
};

} // namespace sdl

namespace img {

class Init {
public:
    Init(int flags);
    ~Init();

    Init(const Init&) = delete;
    Init(Init&&) = delete;
    Init& operator=(const Init&) = delete;
    Init& operator=(Init&&) = delete;
};

sdl::Surface load(const std::filesystem::path& file);
sdl::Surface load(std::span<const std::byte> mem);

} // namespace img

namespace ttf {

class Error : public std::exception {
public:
    Error(std::source_location sl = std::source_location::current());
    const char* what() const noexcept override;

private:
    std::string _message;
};

inline void check(
    int returnCode, std::source_location sl = std::source_location::current())
{
    if (returnCode != 0) {
        throw Error{sl};
    }
}

template <class T>
T* check(T* ptr, std::source_location sl = std::source_location::current())
{
    if (ptr == nullptr) {
        throw Error{sl};
    }
    return ptr;
}

class Init {
public:
    Init();
    ~Init();

    Init(const Init&) = delete;
    Init(Init&&) = delete;
    Init& operator=(const Init&) = delete;
    Init& operator=(Init&&) = delete;
};

class Font {
public:
    explicit Font(TTF_Font* ptr);
    Font(const std::filesystem::path& file, int ptsize);
    Font(std::span<const std::byte> mem, int ptsize);

    TTF_Font* ptr();
    const TTF_Font* ptr() const;

    void setHinting(int hinting);

    sdl::Surface renderUtf8Blended(const char* text, const SDL_Color& fg);
    sdl::Surface renderUtf8Blended(const std::string& text, const SDL_Color& fg);
    sdl::Surface renderUtf8BlendedWrapped(
        const char* text, const SDL_Color& fg, uint32_t wrapLength);
    sdl::Surface renderUtf8BlendedWrapped(
        const std::string& text, const SDL_Color& fg, uint32_t wrapLength);

    sdl::Surface renderUtf8Lcd(
        const std::string& text, const SDL_Color& fg, const SDL_Color& bg);
    sdl::Surface renderUtf8LcdWrapped(
        const std::string& text,
        const SDL_Color& fg,
        const SDL_Color& bg,
        uint32_t wrapLength);

private:
    sdl::RWops _rw;
    std::unique_ptr<TTF_Font, void(*)(TTF_Font*)> _ptr{
        nullptr, TTF_CloseFont};
};

} // namespace ttf