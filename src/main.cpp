#include <cmath>
#include <vector>

#include "model.h"
#include "tga.h"
#include "util.h"
#include "vec3.h"

void DrawLine(int32_t x0, int32_t y0, int32_t x1, int32_t y1, TGAImage& image, const TGAColor& color)
{
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

void DrawTraiangle(Vec3i t0, Vec3i t1, Vec3i t2, TGAImage& image, const TGAColor& color)
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
        Vec3i a = t0 + (t2 - t0) * alpha;
        Vec3i b;
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

int main(int argc, char** argv)
{
    TGAColor white = TGAColor(255, 255, 255, 255);
    TGAColor red = TGAColor(255, 0, 0, 255);
    TGAColor green = TGAColor(0, 255, 0, 255);

    TGAImage image(800, 800, TGAFormat::RGB);

    Vec3i t0[] = {Vec3i(10, 70, 0), Vec3i(50, 160, 0), Vec3i(70, 80, 0)};
    Vec3i t1[] = {Vec3i(180, 50, 0), Vec3i(150, 1, 0), Vec3i(70, 180, 0)};
    Vec3i t2[] = {Vec3i(180, 150, 0), Vec3i(120, 160, 0), Vec3i(130, 180, 0)};

    DrawTraiangle(t0[0], t0[1], t0[2], image, red);
    DrawTraiangle(t1[0], t1[1], t1[2], image, white);
    DrawTraiangle(t2[0], t2[1], t2[2], image, green);

    image.FlipVertically();  // i want to have the origin at the left bottom corner of the image
    image.Write("output.tga");
    return 0;
}
