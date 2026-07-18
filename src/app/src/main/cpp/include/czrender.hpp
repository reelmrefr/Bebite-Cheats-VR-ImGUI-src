#pragma once
#include <cstdint>
#include "imgui.h"

// cz was here

namespace cz {

struct framebuffer {
    int w = 0, h = 0;
    uint8_t* px = nullptr;
};

struct fontatlas {
    int w = 0, h = 0;
    const uint8_t* px = nullptr;
};

void rasterizedrawdata(ImDrawData* dd, framebuffer& fb, const fontatlas& atlas);

}
