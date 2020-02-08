#pragma once

#include "config-master.hpp"

float ConfigMaster::min_gameframe_interval = 16.f;	// 60 FPS as default
std::unordered_map<std::string, std::string> ConfigMaster::subconfigs;

