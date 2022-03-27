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
    const TGAColor color = TGAColor(255, 255, 255, 255);
    const int32_t width = 800;
    const int32_t height = 800;
    TGAImage image(width, height, static_cast<uint8_t>(TGAFormat::RGB));
    for (uint32_t i = 0; i < model->GetNumFaces(); ++i) {
        const std::vector<uint32_t>& face = model->GetFace(i);
        for (uint32_t j = 0; j < 3; ++j) {
            const Vec3& v0 = model->GetVert(face[j]);
            const Vec3& v1 = model->GetVert(face[(j + 1) % 3]);
            int32_t x0 = static_cast<int32_t>((v0.x + 1) * width / 2);
            int32_t y0 = static_cast<int32_t>((v0.y + 1) * height / 2);
            int32_t x1 = static_cast<int32_t>((v1.x + 1) * width / 2);
            int32_t y1 = static_cast<int32_t>((v1.y + 1) * height / 2);
            DrawLine(x0, y0, x1, y1, image, color);
        }
    }
    image.FlipVertically();
    image.Write("output.tga");
    delete model;
    return 0;
}
