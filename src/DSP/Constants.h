#pragma once

static inline float clampf(float v, float lo, float hi) {
    return (v < lo) ? lo : (hi < v) ? hi : v;
}
