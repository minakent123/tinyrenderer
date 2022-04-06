#include <cmath>
#include <vector>

#include "model.h"
#include "tga.h"
#include "util.h"
#include "vec2.h"

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

int main(int argc, char** argv)
{
    if (argc != 2) {
        ERRORF("usage: tinyrenderer path_to_obj");
        return -1;
    }

    Model* model = new Model();
    if (!model->Load(argv[1])) {
        ERRORF("failed to load %s", argv[1]);
        delete model;
        return -1;
    }

    const int32_t width = 800;
    const int32_t height = 800;
    TGAImage image(width, height, TGAFormat::RGB);
    Vec3f lightDir(0, 0, -1);

    for (uint32_t i = 0; i < model->GetNumFaces(); ++i) {
        const std::vector<uint32_t>& face = model->GetFace(i);
        Vec2i screenCoords[3];
        Vec3f worldCoords[3];
        for (uint32_t j = 0; j < 3; ++j) {
            const Vec3f& v = model->GetVert(face[j]);
            screenCoords[j] = Vec2i(static_cast<int32_t>((v.x + 1) * width / 2), static_cast<int32_t>((v.y + 1) * height / 2));
            worldCoords[j] = v;
        }
        Vec3f n = (worldCoords[2] - worldCoords[0]).Cross(worldCoords[1] - worldCoords[0]);
        n.Normalize();
        float intensity = n.Dot(lightDir);
        if (intensity > 0) {
            DrawTraiangle(screenCoords[0], screenCoords[1], screenCoords[2], image, TGAColor(static_cast<uint8_t>(intensity * 255), static_cast<uint8_t>(intensity * 255), static_cast<uint8_t>(intensity * 255), 255));
        }
    }

    image.FlipVertically();
    image.Write("output.tga");
    delete model;
    return 0;
}
