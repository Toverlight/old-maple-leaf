#pragma once

#include <filesystem>

#include "config.h"

AppConfig LoadConfig(const std::filesystem::path& filepath);
