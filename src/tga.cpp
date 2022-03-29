#include "tga.h"

#include "util.h"

TGAImage::TGAImage()
{}

TGAImage::TGAImage(uint32_t w, uint32_t h, TGAFormat format)
{
    m_width = w;
    m_height = h;
    m_bytesPP = static_cast<uint8_t>(format);
    uint32_t nbytes = m_width * m_height * m_bytesPP;
    m_data = new uint8_t[nbytes];
    memset(m_data, 0, nbytes);
}

TGAImage::TGAImage(const TGAImage& img)
{
    m_width = img.m_width;
    m_height = img.m_height;
    m_bytesPP = img.m_bytesPP;
    uint32_t nbytes = m_width * m_height * m_bytesPP;
    m_data = new uint8_t[nbytes];
    memcpy(m_data, img.m_data, nbytes);
}

TGAImage::~TGAImage()
{
    ClearData();
}

bool TGAImage::Read(const char* fileName)
{
    ClearData();

    std::ifstream ifs{fileName, std::ios::binary};
    if (!ifs.is_open()) {
        ERRORF("can't open file %s", fileName);
        ifs.close();
        return false;
    }

    TGAHeader header;
    if (!ifs.read(reinterpret_cast<char*>(&header), sizeof(TGAHeader))) {
        ERRORF("an error occured while reading the header");
        ifs.close();
        return false;
    }

    m_width = header.ImageWidth;
    m_height = header.ImageHeight;
    m_bytesPP = header.PixelDepth >> 3;
    TGAFormat format = static_cast<TGAFormat>(m_bytesPP);

    if (m_width == 0 || m_height == 0 || (format != TGAFormat::GrayScale && format != TGAFormat::RGB && format != TGAFormat::RGBA)) {
        ERRORF("bad bpp (or width / height) value");
        ifs.close();
        return false;
    }

    uint32_t nbytes = m_width * m_height * m_bytesPP;
    m_data = new uint8_t[nbytes];

    if (header.ImageType == 2 || header.ImageType == 3) {
        if (!ifs.read(reinterpret_cast<char*>(m_data), nbytes)) {
            ERRORF("an error occured while reading the data");
            ifs.close();
            return false;
        }
    } else if (header.ImageType == 10 || header.ImageType == 11) {
        if (!LoadRLEData(ifs)) {
            ERRORF("an error occured while reading the data");
            ifs.close();
            return false;
        }
    } else {
        ERRORF("unknown file format %u", header.ImageType);
        ifs.close();
        return false;
    }

    if (header.ImageDescriptor & 0x10) {
        FlipHorizontally();
    }

    if (header.ImageDescriptor & 0x20) {
        FlipVertically();
    }

    INFOF("%u x %u / %u", m_width, m_height, m_bytesPP * 8);
    return true;
}

bool TGAImage::Write(const char* fileName, bool rle)
{
    char developerAreaRef[4] = {0, 0, 0, 0};
    char extensionAreaRef[4] = {0, 0, 0, 0};
    char footer[] = {'T', 'R', 'U', 'E', 'V', 'I', 'S', 'I', 'O', 'N', '-', 'X', 'F', 'I', 'L', 'E', '.', '\0'};

    std::ofstream ofs{fileName, std::ios::binary};
    if (!ofs.is_open()) {
        ERRORF("can't open file %s", fileName);
        return false;
    }

    TGAHeader header{};
    header.PixelDepth = m_bytesPP << 3;
    header.ImageWidth = m_width;
    header.ImageHeight = m_height;
    header.ImageType = (static_cast<TGAFormat>(m_bytesPP) == TGAFormat::GrayScale ? (rle ? 11 : 3) : (rle ? 10 : 2));
    header.ImageDescriptor = 0x20;

    if (!ofs.write(reinterpret_cast<char*>(&header), sizeof(header))) {
        ERRORF("can't dump the tga file");
        ofs.close();
        return false;
    }

    if (!rle) {
        if (!ofs.write(reinterpret_cast<char*>(m_data), m_width * m_height * m_bytesPP)) {
            ERRORF("can't unload raw data");
            ofs.close();
            return false;
        }
    } else {
        if (!UnloadRLEData(ofs)) {
            ERRORF("can't unload rle data");
            ofs.close();
            return false;
        }
    }

    if (!ofs.write(developerAreaRef, sizeof(developerAreaRef))) {
        ERRORF("can't dump the tga file");
        ofs.close();
        return false;
    }

    if (!ofs.write(extensionAreaRef, sizeof(extensionAreaRef))) {
        ERRORF("can't dump the tga file");
        ofs.close();
        return false;
    }

    if (!ofs.write(footer, sizeof(extensionAreaRef))) {
        ERRORF("can't dump the tga file");
        ofs.close();
        return false;
    }

    ofs.close();
    return true;
}

void TGAImage::ClearData()
{
    if (m_data != nullptr) {
        delete[] m_data;
        m_data = nullptr;
    }
}

bool TGAImage::LoadRLEData(std::ifstream& ifs)
{
    uint32_t pixelCount = m_width * m_height;
    uint32_t currentPixel = 0;
    uint32_t currentByte = 0;
    TGAColor colorBuffer;

    do {
        uint8_t chunkHeader = ifs.get();
        if (!ifs.good()) {
            ERRORF("an error occured while reading the data");
            return false;
        }
        if (chunkHeader < 128) {
            chunkHeader++;
            for (uint8_t i = 0; i < chunkHeader; ++i) {
                if (!ifs.read(reinterpret_cast<char*>(colorBuffer.raw), m_bytesPP)) {
                    ERRORF("an error occured while reading the header");
                    return false;
                }
                for (uint8_t j = 0; j < m_bytesPP; ++j) {
                    m_data[currentByte++] = colorBuffer.raw[j];
                }
                ++currentPixel;
                if (currentPixel > pixelCount) {
                    ERRORF("too many pixels read");
                    return false;
                }
            }
        } else {
            chunkHeader -= 127;
            if (!ifs.read(reinterpret_cast<char*>(colorBuffer.raw), m_bytesPP)) {
                ERRORF("an error occured while reading the header");
                return false;
            }
            for (uint8_t i = 0; i < chunkHeader; ++i) {
                for (uint8_t j = 0; j < m_bytesPP; ++j) {
                    m_data[currentByte++] = colorBuffer.raw[j];
                }
                ++currentPixel;
                if (currentPixel > pixelCount) {
                    ERRORF("too many pixels read");
                    return false;
                }
            }
        }
    } while (currentPixel < pixelCount);

    return true;
}

bool TGAImage::UnloadRLEData(std::ofstream& ofs)
{
    uint8_t maxChunkLength = 128;
    uint32_t nPixels = m_width * m_height;
    uint32_t curPix = 0;

    while (curPix < nPixels) {
        uint32_t chunkStart = curPix * m_bytesPP;
        uint32_t curByte = curPix * m_bytesPP;
        uint8_t runLength = 1;
        bool raw = true;

        while (curPix + runLength < nPixels && runLength < maxChunkLength) {
            bool succEq = true;
            for (int32_t t = 0; succEq && t < m_bytesPP; ++t) {
                succEq = m_data[curByte + t] == m_data[curByte + t + m_bytesPP];
            }
            curByte += m_bytesPP;
            if (runLength == 1) {
                raw = !succEq;
            }
            if (raw && succEq) {
                runLength--;
                break;
            }
            if (!raw && !succEq) {
                break;
            }
            runLength++;
        }
        curPix += runLength;
        if (!ofs.put(raw ? runLength - 1 : runLength + 127)) {
            ERRORF("can't dump the tga file");
            return false;
        }
        if (!ofs.write(reinterpret_cast<char*>(m_data + chunkStart), raw ? runLength * m_bytesPP : m_bytesPP)) {
            ERRORF("can't dump the tga file");
            return false;
        }
    }
    return true;
}

bool TGAImage::FlipVertically()
{
    if (m_data == nullptr) {
        return false;
    }
    uint32_t bytesPerLine = m_width * m_bytesPP;
    uint8_t* pLine = new uint8_t[bytesPerLine];
    uint32_t half = m_height >> 1;
    for (uint32_t i = 0; i < half; ++i) {
        uint32_t l1 = i * bytesPerLine;
        uint32_t l2 = (m_height - i - 1) * bytesPerLine;
        memcpy(pLine, m_data + l1, bytesPerLine);
        memcpy(m_data + l1, m_data + l2, bytesPerLine);
        memcpy(m_data + l2, pLine, bytesPerLine);
    }
    delete[] pLine;
    return true;
}

bool TGAImage::FlipHorizontally()
{
    if (m_data == nullptr) {
        return false;
    }
    uint32_t half = m_width >> 1;
    for (uint32_t i = 0; i < half; ++i) {
        for (uint32_t j = 0; j < m_height; ++j) {
            const TGAColor& c1 = GetColor(i, j);
            const TGAColor& c2 = GetColor(m_width - i - 1, j);
            SetColor(i, j, c2);
            SetColor(m_width - i - 1, j, c1);
        }
    }
    return true;
}

TGAColor TGAImage::GetColor(uint32_t x, uint32_t y)
{
    if (m_data == nullptr || x >= m_width || y >= m_height) {
        return TGAColor();
    }
    return TGAColor(m_data + (x + y * m_width) * m_bytesPP, m_bytesPP);
}

bool TGAImage::SetColor(uint32_t x, uint32_t y, const TGAColor& c)
{
    if (m_data == nullptr || x >= m_width || y >= m_height) {
        return false;
    }
    memcpy(m_data + (x + y * m_width) * m_bytesPP, c.raw, m_bytesPP);
    return true;
}
