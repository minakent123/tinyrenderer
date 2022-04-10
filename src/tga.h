#pragma once

#include <fstream>

#pragma pack(push, 1)
struct TGAHeader final
{
    uint8_t IDLength;
    uint8_t ColorMapType;
    uint8_t ImageType;
    uint16_t FirstEntryIndex;
    uint16_t ColorMapLength;
    uint8_t ColorMapEntrySize;
    uint16_t XOrigin;
    uint16_t YOrigin;
    uint16_t ImageWidth;
    uint16_t ImageHeight;
    uint8_t PixelDepth;
    uint8_t ImageDescriptor;
};
#pragma pack(pop)

struct TGAColor final
{
    TGAColor()
        : val(0)
        , bytesPP(1)
    {}

    TGAColor(uint8_t r, uint8_t g, uint8_t b, uint8_t A)
        : b(b)
        , g(g)
        , r(r)
        , a(a)
        , bytesPP(4)
    {}

    TGAColor(uint32_t v, uint32_t bpp)
        : val(v)
        , bytesPP(bpp)
    {}

    TGAColor(const TGAColor& c)
        : val(c.val)
        , bytesPP(c.bytesPP)
    {}

    TGAColor(const uint8_t* p, uint32_t bpp)
        : val(0)
        , bytesPP(bpp)
    {
        for (uint32_t i = 0; i < bpp; ++i) {
            raw[i] = p[i];
        }
    }

    TGAColor& operator=(const TGAColor& c)
    {
        bytesPP = c.bytesPP;
        val = c.val;
        return *this;
    }

    union
    {
        struct
        {
            uint8_t b;
            uint8_t g;
            uint8_t r;
            uint8_t a;
        };
        uint8_t raw[4];
        uint32_t val;
    };

    uint32_t bytesPP;
};

enum class TGAFormat
{
    GrayScale = 1,
    RGB = 3,
    RGBA = 4,
};

class TGAImage final
{
public:
    TGAImage();
    TGAImage(uint32_t w, uint32_t h, TGAFormat format);
    TGAImage(const TGAImage& img);
    ~TGAImage();

    bool Initialize();
    bool Read(const char* fileName);
    bool Write(const char* fileName, bool rle = true);
    bool FlipVertically();
    bool FlipHorizontally();
    TGAColor GetColor(uint32_t x, uint32_t y) const;
    bool SetColor(uint32_t x, uint32_t y, const TGAColor& c);
    uint32_t GetWidth() const { return m_width; }
    uint32_t GetHeight() const { return m_height; }

private:
    void ClearData();
    bool LoadRLEData(std::ifstream& ifs);
    bool UnloadRLEData(std::ofstream& ofs);

private:
    uint8_t* m_data{};
    uint32_t m_width{};
    uint32_t m_height{};
    uint8_t m_bytesPP{};
};
