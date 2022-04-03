#include <cmath>
#include <vector>

#include "model.h"
#include "tga.h"
#include "util.h"
#include "vec2.h"
#include "vec3.h"

Vec3f Barycentric(Vec2i* pts, Vec2i p)
{
    Vec3f u = Vec3f(static_cast<float>(pts[2].x - pts[0].x), static_cast<float>(pts[1].x - pts[0].x), static_cast<float>(pts[0].x - p.x)).Cross(Vec3f(static_cast<float>(pts[2].y - pts[0].y), static_cast<float>(pts[1].y - pts[0].y), static_cast<float>(pts[0].y - p.y)));
    if (std::abs(u.z) < 1) {
        return Vec3f(-1, 1, 1);
    }
    return Vec3f(1.f - (u.x + u.y) / u.z, u.y / u.z, u.x / u.z);
}

void DrawTraiangle(Vec2i* pts, TGAImage& image, const TGAColor& color)
{
    Vec2i bboxmin(image.GetWidth() - 1, image.GetHeight() - 1);
    Vec2i bboxmax(0, 0);
    Vec2i clamp(image.GetWidth() - 1, image.GetHeight() - 1);

    for (int32_t i = 0; i < 3; ++i) {
        bboxmin.x = std::max(0, std::min(bboxmin.x, pts[i].x));
        bboxmin.y = std::max(0, std::min(bboxmin.y, pts[i].y));
        bboxmax.x = std::min(clamp.x, std::max(bboxmax.x, pts[i].x));
        bboxmax.y = std::min(clamp.y, std::max(bboxmax.y, pts[i].y));
    }

    Vec2i p;
    for (p.x = bboxmin.x; p.x <= bboxmax.x; ++p.x) {
        for (p.y = bboxmin.y; p.y <= bboxmax.y; ++p.y) {
            Vec3f bc_screen = Barycentric(pts, p);
            if (bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z < 0) {
                continue;
            }
            image.SetColor(p.x, p.y, color);
        }
    }
}

int main(int argc, char** argv)
{
    TGAColor red = TGAColor(255, 0, 0, 255);
    TGAImage image(200, 200, TGAFormat::RGB);
    Vec2i pts[3] = {Vec2i(10, 10), Vec2i(100, 30), Vec2i(190, 160)};
    DrawTraiangle(pts, image, red);
    image.FlipVertically();
    image.Write("output.tga");
    return 0;
}
