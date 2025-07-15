#pragma once

namespace dx3d
{
    struct Vec2
    {
        float x = 0, y = 0;

        Vec2() = default;
        Vec2(float x, float y) : x(x), y(y) {}
    };
}