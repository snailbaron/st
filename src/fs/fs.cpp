#include "fs.hpp"

#include <error.hpp>

#ifdef _WIN32
    #include <Shlobj.h>
    #include <Windows.h>
#endif

#include <stdexcept>

namespace fs {

namespace {

void throwWindowsError()
{
    auto errorCode = GetLastError();
    char* buffer = nullptr;
    auto charsWritten = FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL /*lpSource*/,
        errorCode,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPSTR)&buffer,
        0 /*nSize*/,
        NULL /*Arguments*/);
    if (charsWritten == 0) {
        throw e::Error{} << "error: " << errorCode;
    }

    auto error = e::Error{};
    error << buffer;
    LocalFree(buffer);

    throw error;
}

} // namespace

std::filesystem::path home()
{
#ifdef _WIN32
    PWSTR path = nullptr;
    if (SHGetKnownFolderPath(
            FOLDERID_Profile,
            KF_FLAG_DEFAULT,
            NULL,
            &path) != S_OK) {
        throwWindowsError();
    }
    return std::filesystem::path{path};
#endif
}

std::filesystem::path exe()
{
#ifdef _WIN32
    auto path = std::string(100, '\0');
    auto l = GetModuleFileName(NULL, path.data(), (DWORD)path.size());
    if (l == 0) {
        throwWindowsError();
    }

    while (l == path.size()) {
        path.resize(path.size() * 2);
        l = GetModuleFileName(NULL, path.data(), (DWORD)path.size());
        if (l == 0) {
            throwWindowsError();
        }
    }

    return path;
#endif
}

std::filesystem::path exeDir()
{
    return exe().parent_path();
}

std::string readText(const std::filesystem::path& path)
{
    auto input = std::ifstream{path, std::ios::binary};
    input.exceptions(std::ios::badbit | std::ios::failbit);

    auto contents = std::ostringstream{};
    contents << input.rdbuf();
    input.close();

    return contents.str();
}

std::vector<std::byte> readBytes(const std::filesystem::path& path)
{
    auto input = std::ifstream{path, std::ios::binary | std::ios::ate};
    input.exceptions(std::ios::badbit | std::ios::failbit);
    auto fileSize = input.tellg();
    auto bytes = std::vector<std::byte>(fileSize);
    input.seekg(0);
    input.read(reinterpret_cast<char*>(bytes.data()), fileSize);
    return bytes;
}

MemoryMap::MemoryMap(const std::filesystem::path& path)
{
    open(path);
}

MemoryMap::MemoryMap(MemoryMap&& other) noexcept
{
    std::swap(*this, other);
}

MemoryMap& MemoryMap::operator=(MemoryMap&& other) noexcept
{
    if (this != &other) {
        close();
        std::swap(*this, other);
    }
    return *this;
}

void MemoryMap::open(const std::filesystem::path& path)
{
#ifdef _WIN32
    auto fileHandle = CreateFileW(
        path.c_str(),
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL);
    if (fileHandle == INVALID_HANDLE_VALUE) {
        throwWindowsError();
    }
    _fileHandle = fileHandle;

    auto fileSize = LARGE_INTEGER{};
    if (GetFileSizeEx(_fileHandle, &fileSize) == 0) {
        throwWindowsError();
    }
    _size = fileSize.QuadPart;

    HANDLE mappingHandle = CreateFileMappingA(
        _fileHandle,
        NULL,
        PAGE_READONLY,
        0,
        0,
        NULL);
    if (mappingHandle == NULL) {
        throwWindowsError();
    }
    _mappingHandle = mappingHandle;

    void* address = MapViewOfFile(_mappingHandle, FILE_MAP_READ, 0, 0, 0);
    if (address == NULL) {
        throwWindowsError();
    }
    _address = reinterpret_cast<const std::byte*>(address);
#endif
}

void MemoryMap::close()
{
#ifdef _WIN32
    if (_address) {
        UnmapViewOfFile(_address);
        _address = nullptr;

        CloseHandle(_mappingHandle);
        _mappingHandle = nullptr;

        CloseHandle(_fileHandle);
        _fileHandle = nullptr;

        _size = 0;
    }
#endif
}

std::span<const std::byte> MemoryMap::bytes() const
{
    return {_address, _size};
}

size_t MemoryMap::size() const
{
    return _size;
}

void swap(MemoryMap& lhs, MemoryMap& rhs) noexcept
{
#ifdef _WIN32
    std::swap(lhs._fileHandle, rhs._fileHandle);
#endif
}

} // namespace fs