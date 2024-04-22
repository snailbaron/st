#pragma once

#include <cstddef>
#include <filesystem>
#include <fstream>
#include <span>
#include <sstream>
#include <vector>

namespace fs {

std::filesystem::path home();
std::filesystem::path exe();
std::filesystem::path exeDir();

std::string readText(const std::filesystem::path& path);
std::vector<std::byte> readBytes(const std::filesystem::path& path);

class MemoryMap {
public:
    MemoryMap() = default;
    explicit MemoryMap(const std::filesystem::path& path);

    MemoryMap(MemoryMap&& other) noexcept;
    MemoryMap& operator=(MemoryMap&& other) noexcept;

    MemoryMap(const MemoryMap&) = delete;
    MemoryMap& operator=(const MemoryMap&) = delete;

    void open(const std::filesystem::path& path);
    void close();

    std::span<const std::byte> bytes() const;

    template <class T = std::byte>
    const T* address() const
    {
        static_assert(sizeof(T) == sizeof(std::byte));
        return reinterpret_cast<const T*>(_address);
    }

    size_t size() const;

    friend void swap(MemoryMap& lhs, MemoryMap& rhs) noexcept;

private:
#ifdef _WIN32
    void* _fileHandle = nullptr;
    void* _mappingHandle = nullptr;
#endif

    const std::byte* _address = nullptr;
    size_t _size = 0;
};

} // namespace fs