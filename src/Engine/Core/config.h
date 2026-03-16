#pragma once

#include <string>

struct WindowConfig
{
    int width = 1280;
    int height = 720;
    std::string title = "MapleLeaf";

    bool fullscreen = false;
    bool resizable = true;
};

struct FrameConfig
{
    int swapInterval = 1; // 0 = vsync off, 1 = vsync on, >1 = sync every N frames
    int maxFps = 0; // 0 = no limit
    bool vsync = true;
    int spinWaitUs = 500; // Used when maxFps>0 and vsync is off. 0 disables spin.
};

class AppConfig
{
public:
    WindowConfig windowConfig;
    FrameConfig frameConfig;
};