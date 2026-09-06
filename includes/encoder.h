#pragma once
#include <string>
#include "bitbuffer.h"

BitBuffer encodeByteMode(const std::string& text, int capacityBits);