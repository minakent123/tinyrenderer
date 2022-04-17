#include <cmath>
#include <limits>
#include <vector>

#include "model.h"
#include "tga.h"
#include "util.h"
#include "vec2.h"

static const int32_t WIDTH = 800;
static const int32_t HEIGHT = 500;
static const int32_t DEPTH = 255;

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
void DrawTraiangle(Vec3i t0, Vec3i t1, Vec3i t2, int32_t* zbuffer, TGAImage& image, const TGAColor& color)
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
            float phi = b.x == a.x ? 1.f : (float)(j - a.x) / (float)(b.x - a.x);
            Vec3i p = a + (b - a) * phi;
            p.x = j;
            p.y = t0.y + i;  // a hack to fill holes (due to int cast precision problems)
            int32_t idx = j + (t0.y + i) * WIDTH;
            if (zbuffer[idx] < p.z) {
                zbuffer[idx] = p.z;
                image.SetColor(p.x, p.y, color);
            }
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

Vec3f World2Screen(const Vec3f& v)
{
    return Vec3f((v.x + 1.f) * WIDTH / 2 + 0.5f, (v.y + 1.f) * HEIGHT / 2 + 0.5f, v.z);
}

int main(int argc, char** argv)
{
    Model* model = new Model();
    model->Load("african_head.obj");
    int32_t* zbuffer = new int32_t[WIDTH * HEIGHT];
    for (int32_t i = 0; i < WIDTH * HEIGHT; ++i) {
        zbuffer[i] = std::numeric_limits<int32_t>::min();
    }

    TGAImage image(WIDTH, HEIGHT, TGAFormat::RGB);
    Vec3f light_dir(0, 0, -1.f);
    for (uint32_t i = 0; i < model->GetNumFaces(); ++i) {
        const std::vector<uint32_t>& face = model->GetFace(i);
        Vec3i screen_coords[3];
        Vec3f world_coords[3];
        for (int32_t i = 0; i < 3; ++i) {
            const Vec3f& v = model->GetVert(face[i]);
            screen_coords[i] = Vec3i(static_cast<int32_t>((v.x + 1.f) * WIDTH / 2), static_cast<int32_t>((v.y + 1.f) * HEIGHT / 2), static_cast<int32_t>((v.z + 1.f) * DEPTH / 2));
            world_coords[i] = v;
        }
        Vec3f n = (world_coords[2] - world_coords[0]).Cross(world_coords[1] - world_coords[0]);
        n.Normalize();
        float intensity = n.Dot(light_dir);
        if (intensity > 0) {
            DrawTraiangle(screen_coords[0], screen_coords[1], screen_coords[2], zbuffer, image, TGAColor(static_cast<uint8_t>(intensity * 255), static_cast<uint8_t>(intensity * 255), static_cast<uint8_t>(intensity * 255), 255));
        }
    }
    image.FlipVertically();
    image.Write("output.tga");
    delete model;
    return 0;
}
