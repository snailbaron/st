#include <sdl.hpp>

#include <atomic>
#include <sstream>
#include <stacktrace>

namespace sdl {

namespace {

std::atomic_int initCount = 0;

} // namespace

Error::Error(std::source_location sl)
{
    auto stream = std::ostringstream{};
    stream <<
        std::stacktrace::current() <<
        sl.file_name() << ": " <<
        sl.line() << ":" << sl.column() <<
        " (" << sl.function_name() << "): " <<
        SDL_GetError();
    _message = std::move(stream).str();
}

const char* Error::what() const noexcept
{
    return _message.c_str();
}

Init::Init(uint32_t flags)
{
    if (initCount++ == 0) {
        check(SDL_Init(flags));
    }
}

Init::~Init()
{
    if (--initCount == 0) {
        SDL_Quit();
    }
}

RWops::RWops(SDL_RWops* ptr)
{
    _ptr.reset(ptr);
}

RWops::RWops(const std::filesystem::path& file, const char* mode)
{
    _ptr.reset(check(SDL_RWFromFile(file.string().c_str(), mode)));
}

RWops::RWops(std::span<const std::byte> mem)
{
    _ptr.reset(check(SDL_RWFromConstMem(mem.data(), (int)mem.size())));
}

SDL_RWops* RWops::ptr()
{
    return _ptr.get();
}

const SDL_RWops* RWops::ptr() const
{
    return _ptr.get();
}

Surface::Surface(SDL_Surface* ptr)
{
    _ptr.reset(ptr);
}

Texture::Texture(SDL_Texture* ptr)
{
    _ptr.reset(ptr);
}

SDL_Texture* Texture::ptr()
{
    return _ptr.get();
}

const SDL_Texture* Texture::ptr() const
{
    return _ptr.get();
}

SDL_Surface* Surface::ptr()
{
    return _ptr.get();
}

const SDL_Surface* Surface::ptr() const
{
    return _ptr.get();
}

SDL_Surface& Surface::operator*()
{
    return *_ptr;
}

const SDL_Surface& Surface::operator*() const
{
    return *_ptr;
}

SDL_Surface* Surface::operator->()
{
    return _ptr.get();
}

const SDL_Surface* Surface::operator->() const
{
    return _ptr.get();
}

Window::Window(const char* title, int x, int y, int w, int h, uint32_t flags)
{
    _ptr.reset(check(SDL_CreateWindow(title, x, y, w, h, flags)));
}

SDL_Window* Window::ptr()
{
    return _ptr.get();
}

const SDL_Window* Window::ptr() const
{
    return _ptr.get();
}

Renderer::Renderer(Window& window, int index, uint32_t flags)
{
    _ptr.reset(check(SDL_CreateRenderer(window.ptr(), index, flags)));
}

SDL_Renderer* Renderer::ptr()
{
    return _ptr.get();
}

const SDL_Renderer* Renderer::ptr() const
{
    return _ptr.get();
}

Texture Renderer::createTextureFromSurface(Surface& surface)
{
    return Texture{check(SDL_CreateTextureFromSurface(ptr(), surface.ptr()))};
}

Texture Renderer::createTextureFromSurface(Surface&& surface)
{
    return Texture{check(SDL_CreateTextureFromSurface(ptr(), surface.ptr()))};
}

Texture Renderer::loadTexture(const std::filesystem::path& file)
{
    return Texture{check(IMG_LoadTexture(ptr(), file.string().c_str()))};
}

Texture Renderer::loadTexture(std::span<const std::byte> mem)
{
    auto rwops = RWops{mem};
    return Texture{check(IMG_LoadTexture_RW(ptr(), rwops.ptr(), 0))};
}

void Renderer::clear()
{
    check(SDL_RenderClear(ptr()));
}

void Renderer::present()
{
    SDL_RenderPresent(ptr());
}

void Renderer::setDrawColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    check(SDL_SetRenderDrawColor(ptr(), r, g, b, a));
}

void Renderer::setDrawColor(const SDL_Color& color)
{
    setDrawColor(color.r, color.g, color.b, color.a);
}

void Renderer::copy(
    Texture& texture, const SDL_Rect* srcrect, const SDL_Rect* dstrect)
{
    check(SDL_RenderCopy(ptr(), texture.ptr(), srcrect, dstrect));
}

void Renderer::copy(
    Texture& texture, const SDL_Rect* srcrect, const SDL_FRect* dstrect)
{
    check(SDL_RenderCopyF(ptr(), texture.ptr(), srcrect, dstrect));
}

void Renderer::copy(Texture& texture, const SDL_Rect& srcrect, const SDL_FRect& dstrect)
{
    check(SDL_RenderCopyF(ptr(), texture.ptr(), &srcrect, &dstrect));
}

void Renderer::copy(Texture& texture, const SDL_FRect& dstrect)
{
    check(SDL_RenderCopyF(ptr(), texture.ptr(), nullptr, &dstrect));
}

void Renderer::drawPoint(int x, int y)
{
    check(SDL_RenderDrawPoint(ptr(), x, y));
}

void Renderer::drawPoint(float x, float y)
{
    check(SDL_RenderDrawPointF(ptr(), x, y));
}

void Renderer::drawPoints(std::span<const SDL_Point> points)
{
    check(SDL_RenderDrawPoints(ptr(), points.data(), (int)points.size()));
}

void Renderer::drawPoints(std::span<const SDL_FPoint> points)
{
    check(SDL_RenderDrawPointsF(ptr(), points.data(), (int)points.size()));
}

void Renderer::drawLine(int x1, int y1, int x2, int y2)
{
    check(SDL_RenderDrawLine(ptr(), x1, y1, x2, y2));
}

void Renderer::drawLine(float x1, float y1, float x2, float y2)
{
    check(SDL_RenderDrawLineF(ptr(), x1, y1, x2, y2));
}

void Renderer::drawLines(std::span<const SDL_Point> points)
{
    check(SDL_RenderDrawLines(ptr(), points.data(), (int)points.size()));
}

void Renderer::drawLines(std::span<const SDL_FPoint> points)
{
    check(SDL_RenderDrawLinesF(ptr(), points.data(), (int)points.size()));
}

void Renderer::drawRect(const SDL_Rect& rect)
{
    check(SDL_RenderDrawRect(ptr(), &rect));
}

void Renderer::drawRect(const SDL_FRect& rect)
{
    check(SDL_RenderDrawRectF(ptr(), &rect));
}

void Renderer::drawRects(std::span<const SDL_Rect> rects)
{
    check(SDL_RenderDrawRects(ptr(), rects.data(), (int)rects.size()));
}

void Renderer::drawRects(std::span<const SDL_FRect> rects)
{
    check(SDL_RenderDrawRectsF(ptr(), rects.data(), (int)rects.size()));
}

void Renderer::fillRect(const SDL_Rect& rect)
{
    check(SDL_RenderFillRect(ptr(), &rect));
}

void Renderer::fillRect(const SDL_FRect& rect)
{
    check(SDL_RenderFillRectF(ptr(), &rect));
}

void Renderer::fillRects(std::span<const SDL_Rect> rects)
{
    check(SDL_RenderFillRects(ptr(), rects.data(), (int)rects.size()));
}

void Renderer::fillRects(std::span<const SDL_FRect> rects)
{
    check(SDL_RenderFillRectsF(ptr(), rects.data(), (int)rects.size()));
}

void Renderer::renderGeometry(std::span<const SDL_Vertex> vertices)
{
    check(SDL_RenderGeometry(
        ptr(), nullptr, vertices.data(), (int)vertices.size(), nullptr, 0));
}

void Renderer::renderGeometry(
    Texture& texture, std::span<const SDL_Vertex> vertices)
{
    check(SDL_RenderGeometry(
        ptr(), texture.ptr(), vertices.data(), (int)vertices.size(), nullptr, 0));
}

void Renderer::renderGeometry(
    std::span<const SDL_Vertex> vertices, std::span<const int> indices)
{
    check(SDL_RenderGeometry(
        ptr(),
        nullptr,
        vertices.data(),
        (int)vertices.size(),
        indices.data(),
        (int)indices.size()));
}

void Renderer::renderGeometry(
    Texture& texture,
    std::span<const SDL_Vertex> vertices,
    std::span<const int> indices)
{
    check(SDL_RenderGeometry(
        ptr(),
        texture.ptr(),
        vertices.data(),
        (int)vertices.size(),
        indices.data(),
        (int)indices.size()));
}

} // namespace sdl

namespace img {

namespace {

std::atomic_int initCount = 0;

} // namespace

Init::Init(int flags)
{
    if (initCount++ == 0) {
        if (IMG_Init(flags) != flags) {
            throw sdl::Error{};
        }
    }
}

Init::~Init()
{
    if (--initCount == 0) {
        IMG_Quit();
    }
}

sdl::Surface load(const std::filesystem::path& file)
{
    return sdl::Surface{sdl::check(IMG_Load(file.string().c_str()))};
}

sdl::Surface load(std::span<const std::byte> mem)
{
    auto rwops = sdl::RWops{mem};
    return sdl::Surface{sdl::check(IMG_Load_RW(rwops.ptr(), 0))};
}

} // namespace img

namespace ttf {

namespace {

std::atomic_int initCount = 0;

} // namespace

Error::Error(std::source_location sl)
{
    auto stream = std::ostringstream{};
    stream <<
        std::stacktrace::current() <<
        sl.file_name() << ": " <<
        sl.line() << ":" << sl.column() <<
        " (" << sl.function_name() << "): " <<
        TTF_GetError();
    _message = std::move(stream).str();
}

const char* Error::what() const noexcept
{
    return _message.c_str();
}

Init::Init()
{
    if (initCount++ == 0) {
        check(TTF_Init());
    }
}

Init::~Init()
{
    if (--initCount == 0) {
        TTF_Quit();
    }
}

Font::Font(TTF_Font* ptr)
{
    _ptr.reset(ptr);
}

Font::Font(const std::filesystem::path& file, int ptsize)
{
    _ptr.reset(check(TTF_OpenFont(file.string().c_str(), ptsize)));
}

Font::Font(std::span<const std::byte> mem, int ptsize)
{
    _rw = sdl::RWops{mem};
    _ptr.reset(check(TTF_OpenFontRW(_rw.ptr(), 0, ptsize)));
}

TTF_Font* Font::ptr()
{
    return _ptr.get();
}

const TTF_Font* Font::ptr() const
{
    return _ptr.get();
}

void Font::setHinting(int hinting)
{
    TTF_SetFontHinting(ptr(), hinting);
}

sdl::Surface Font::renderUtf8Blended(const char* text, const SDL_Color& fg)
{
    return sdl::Surface{check(TTF_RenderUTF8_Blended(ptr(), text, fg))};
}

sdl::Surface Font::renderUtf8Blended(const std::string& text, const SDL_Color& fg)
{
    return renderUtf8Blended(text.c_str(), fg);
}

sdl::Surface Font::renderUtf8BlendedWrapped(
    const char* text, const SDL_Color& fg, uint32_t wrapLength)
{
    return sdl::Surface{check(TTF_RenderUTF8_Blended_Wrapped(
        ptr(), text, fg, wrapLength))};
}

sdl::Surface Font::renderUtf8BlendedWrapped(
    const std::string& text, const SDL_Color& fg, uint32_t wrapLength)
{
    return renderUtf8BlendedWrapped(text.c_str(), fg, wrapLength);
}

sdl::Surface Font::renderUtf8Lcd(
    const std::string& text, const SDL_Color& fg, const SDL_Color& bg)
{
    return sdl::Surface{check(TTF_RenderUTF8_LCD(ptr(), text.c_str(), fg, bg))};
}

sdl::Surface Font::renderUtf8LcdWrapped(
    const std::string& text,
    const SDL_Color& fg,
    const SDL_Color& bg,
    uint32_t wrapLength)
{
    return sdl::Surface{check(TTF_RenderUTF8_LCD_Wrapped(
        ptr(), text.c_str(), fg, bg, wrapLength))};
}

} // namespace ttf