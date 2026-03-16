#include "parser.h"

#include "file.h"

#include <algorithm>
#include <cctype>
#include <optional>
#include <string>
#include <string_view>

namespace {

void TrimInPlace(std::string& s)
{
    auto isSpace = [](unsigned char ch) { return std::isspace(ch) != 0; };
    while (!s.empty() && isSpace(static_cast<unsigned char>(s.front()))) {
        s.erase(s.begin());
    }
    while (!s.empty() && isSpace(static_cast<unsigned char>(s.back()))) {
        s.pop_back();
    }
}

void ToLowerInPlace(std::string& s)
{
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
}

void StripInlineCommentInPlace(std::string& s)
{
    // Full-line comments (# or ;) and inline comments when preceded by whitespace.
    for (size_t i = 0; i < s.size(); ++i) {
        const char ch = s[i];
        if (ch != '#' && ch != ';') {
            continue;
        }

        if (i == 0) {
            s.clear();
            return;
        }

        if (std::isspace(static_cast<unsigned char>(s[i - 1])) != 0) {
            s.erase(i);
            return;
        }
    }
}

void UnquoteInPlace(std::string& s)
{
    if (s.size() >= 2) {
        const char a = s.front();
        const char b = s.back();
        if ((a == '"' && b == '"') || (a == '\'' && b == '\'')) {
            s = s.substr(1, s.size() - 2);
        }
    }
}

std::optional<bool> ParseBool(std::string s)
{
    TrimInPlace(s);
    ToLowerInPlace(s);

    if (s == "1" || s == "true" || s == "yes" || s == "on") return true;
    if (s == "0" || s == "false" || s == "no" || s == "off") return false;
    return std::nullopt;
}

std::optional<int> ParseInt(std::string s)
{
    TrimInPlace(s);
    if (s.empty()) return std::nullopt;

    size_t idx = 0;
    try {
        const int v = std::stoi(s, &idx, 10);
        if (idx != s.size()) return std::nullopt;
        return v;
    } catch (...) {
        return std::nullopt;
    }
}

} // namespace

AppConfig LoadConfig(const std::filesystem::path& filepath)
{
    AppConfig config;

    auto textRes = ReadTextFileUtf8(filepath);
    if (!textRes) {
        // Missing config is not an error: defaults are valid.
        if (textRes.error != FileReadError::OpenFailed) {
            std::cerr << "Failed to read config file: " << filepath << " (error: " << FileReadErrorToString(textRes.error) << ")" << std::endl;
        }
        return config;
    }

    std::string section;
    bool sawVsync = false;
    bool sawSwapInterval = false;

    std::string_view textView(textRes.data);
    size_t lineStart = 0;
    while (lineStart <= textView.size()) {
        const size_t lineEnd = textView.find_first_of("\r\n", lineStart);
        std::string line = std::string(textView.substr(lineStart, (lineEnd == std::string_view::npos) ? std::string_view::npos : (lineEnd - lineStart)));

        // Advance to next line (handle \r\n, \n, \r)
        if (lineEnd == std::string_view::npos) {
            lineStart = textView.size() + 1;
        } else {
            lineStart = lineEnd + 1;
            if (lineStart < textView.size() && textView[lineEnd] == '\r' && textView[lineStart] == '\n') {
                ++lineStart;
            }
        }

        StripInlineCommentInPlace(line);
        TrimInPlace(line);
        if (line.empty()) {
            continue;
        }

        if (line.front() == '[' && line.back() == ']') {
            section = line.substr(1, line.size() - 2);
            TrimInPlace(section);
            ToLowerInPlace(section);
            continue;
        }

        const size_t eqPos = line.find('=');
        if (eqPos == std::string::npos) {
            continue;
        }

        std::string key = line.substr(0, eqPos);
        std::string value = line.substr(eqPos + 1);
        TrimInPlace(key);
        TrimInPlace(value);
        ToLowerInPlace(key);

        if (section == "window") {
            if (key == "width") {
                if (auto v = ParseInt(value)) config.windowConfig.width = std::max(1, *v);
            } else if (key == "height") {
                if (auto v = ParseInt(value)) config.windowConfig.height = std::max(1, *v);
            } else if (key == "title") {
                UnquoteInPlace(value);
                if (!value.empty()) config.windowConfig.title = value;
            } else if (key == "fullscreen") {
                if (auto v = ParseBool(value)) config.windowConfig.fullscreen = *v;
            } else if (key == "resizable") {
                if (auto v = ParseBool(value)) config.windowConfig.resizable = *v;
            }
            continue;
        }

        if (section == "frame") {
            if (key == "vsync") {
                if (auto v = ParseBool(value)) {
                    config.frameConfig.vsync = *v;
                    sawVsync = true;
                }
            } else if (key == "swapinterval") {
                if (auto v = ParseInt(value)) {
                    config.frameConfig.swapInterval = std::max(0, *v);
                    config.frameConfig.vsync = (config.frameConfig.swapInterval > 0);
                    sawSwapInterval = true;
                }
            } else if (key == "maxfps") {
                if (auto v = ParseInt(value)) config.frameConfig.maxFps = std::max(0, *v);
            }
            continue;
        }
    }

    // Precedence: if vsync specified but swapInterval isn't, map vsync to interval 1/0.
    if (sawVsync && !sawSwapInterval) {
        config.frameConfig.swapInterval = config.frameConfig.vsync ? 1 : 0;
    }

    return config;
}