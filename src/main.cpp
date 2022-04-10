#include <cmath>
#include <limits>
#include <vector>

#include "model.h"
#include "tga.h"
#include "util.h"
#include "vec2.h"

void DrawLine(const Vec2i& p0, const Vec2i& p1, TGAImage& image, const TGAColor& color)
{
    int32_t x0 = p0.x;
    int32_t x1 = p1.x;
    int32_t y0 = p0.y;
    int32_t y1 = p1.y;
    bool steep = false;
    if (std::abs(x0 - x1) < std::abs(y0 - y1)) {
        std::swap(x0, y0);
        std::swap(x1, y1);
        steep = true;
    }
    if (x0 > x1) {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }
    int32_t dx = x1 - x0;
    int32_t dy = y1 - y0;
    int32_t derror = std::abs(dy) * 2;
    int32_t error = 0;
    int32_t y = y0;
    for (int32_t x = x0; x <= x1; ++x) {
        if (steep) {
            image.SetColor(y, x, color);
        } else {
            image.SetColor(x, y, color);
        }
        error += derror;
        if (error > dx) {
            y += (y0 < y1 ? 1 : -1);
            error -= dx * 2;
        }
    }
}

void DrawTraiangle(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage& image, const TGAColor& color)
{
    if (t0.y == t1.y && t0.y == t2.y) {
        return;
    }
    if (t0.y > t1.y) {
        std::swap(t0, t1);
    }
    if (t0.y > t2.y) {
        std::swap(t0, t2);
    }
    if (t1.y > t2.y) {
        std::swap(t1, t2);
    }
    int32_t total_height = t2.y - t0.y;
    for (int32_t i = 0; i < total_height; ++i) {
        float alpha = static_cast<float>(i) / total_height;
        Vec2i a = t0 + (t2 - t0) * alpha;
        Vec2i b;
        bool second_half = (i > t1.y - t0.y) || (t1.y == t0.y);
        if (second_half) {
            int32_t segment_height = t2.y - t1.y;
            float beta = static_cast<float>(i - (t1.y - t0.y)) / segment_height;
            b = t1 + (t2 - t1) * beta;
        } else {
            int32_t segment_height = t1.y - t0.y;
            float beta = static_cast<float>(i) / segment_height;
            b = t0 + (t1 - t0) * beta;
        }
        if (a.x > b.x) {
            std::swap(a, b);
        }
        for (int32_t j = a.x; j < b.x; ++j) {
            image.SetColor(j, t0.y + i, color);
        }
    }
}

void Rasterize(Vec2i p0, Vec2i p1, TGAImage& image, const TGAColor& color, int32_t ybuffer[])
{
    if (p0.x > p1.x) {
        std::swap(p0, p1);
    }
    for (int32_t x = p0.x; x <= p1.x; ++x) {
        float t = static_cast<float>(x - p0.x) / (p1.x - p0.x);
        int32_t y = static_cast<int32_t>(p0.y * (1 - t) + p1.y * t + 0.5f);
        if (ybuffer[x] < y) {
            ybuffer[x] = y;
            image.SetColor(x, 0, color);
        }
    }
}

int main(int argc, char** argv)
{
    const int32_t width = 800;
    const int32_t height = 500;
    const TGAColor white = TGAColor(255, 255, 255, 255);
    const TGAColor red = TGAColor(255, 0, 0, 255);
    const TGAColor green = TGAColor(0, 255, 0, 255);
    const TGAColor blue = TGAColor(0, 0, 255, 255);

    {
        TGAImage scene(width, height, TGAFormat::RGB);

        DrawLine(Vec2i(20, 34), Vec2i(744, 400), scene, red);
        DrawLine(Vec2i(120, 434), Vec2i(444, 400), scene, green);
        DrawLine(Vec2i(330, 463), Vec2i(594, 200), scene, blue);

        DrawLine(Vec2i(10, 10), Vec2i(790, 10), scene, white);

        scene.FlipVertically();
        scene.Write("scene.tga");
    }
    {
        TGAImage render(width, 16, TGAFormat::RGB);
        int32_t ybuffer[width];
        for (int32_t i = 0; i < width; ++i) {
            ybuffer[i] = std::numeric_limits<int>::min();
        }
        Rasterize(Vec2i(20, 34), Vec2i(744, 400), render, red, ybuffer);
        Rasterize(Vec2i(120, 434), Vec2i(444, 400), render, green, ybuffer);
        Rasterize(Vec2i(330, 463), Vec2i(594, 200), render, blue, ybuffer);

        for (int32_t i = 0; i < width; ++i) {
            for (int32_t j = 1; j < 16; ++j) {
                render.SetColor(i, j, render.GetColor(i, 0));
            }
        }

        render.FlipVertically();
        render.Write("render.tga");
    }
    return 0;
}
