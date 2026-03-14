#pragma once

#include <vector>
#include <cstdint>
#include <filesystem>
#include <string>
#include <fstream>
#include <iostream>

enum class FileReadError {
    None,
    OpenFailed,
    TellFailed,
    ReadFailure
};

template <class T>
struct _FileReadResult {
    T data{};
    FileReadError error = FileReadError::None;
    explicit operator bool() const { return error == FileReadError::None; }
};

using FileBytesResult = _FileReadResult<std::vector<std::uint8_t>>;
using FileTextResult = _FileReadResult<std::string>;

inline FileBytesResult ReadFileBytes(const std::filesystem::path& filepath) {
    std::ifstream file(filepath, std::ios::binary | std::ios::ate);
    if (!file) {
        return {{}, FileReadError::OpenFailed};
    }

    const std::streamsize size = file.tellg();
    if (size < 0) {
        return {{}, FileReadError::TellFailed};
    }

    std::vector<std::uint8_t> bytes(static_cast<size_t>(size));
    file.seekg(0, std::ios::beg);

    if (size > 0 && !file.read(reinterpret_cast<char*>(bytes.data()), size)) {
         return {{}, FileReadError::ReadFailure};
    }

    return {std::move(bytes), FileReadError::None};
}

inline FileTextResult ReadTextFileUtf8(const std::filesystem::path& filepath) {
    auto bytesRes = ReadFileBytes(filepath);
    if (!bytesRes) { return {{}, bytesRes.error}; }
    
    std::string text(bytesRes.data.begin(), bytesRes.data.end());

    if (text.size() >= 3 && static_cast<unsigned char>(text[0]) == 0xEF && static_cast<unsigned char>(text[1]) == 0xBB && static_cast<unsigned char>(text[2]) == 0xBF) {
        text.erase(0, 3);
    }

    return {std::move(text), FileReadError::None};
}