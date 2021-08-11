/*
* Created by iabrooks on May 3, 2021
*
* This file contains code for the easy creation and saving of PNG images
* using the publicly available "STB Image" library.
*/

#ifndef ISARCUS_IMAGE_HPP
#include "../Image.hpp"
#endif

#include "../image_math.hpp"

#include "stb/stb_image_write.h"
#include "stb/stb_image.h"

#include <cmath>
#include <iostream>
#include <fstream>

#define LOOP_IMAGE for (int x = 0; x < width; x++) for (int y = 0; y < height; y++)

namespace zimg
{

constexpr double PI = 3.141592653589793238462643383279502884197169399375105;

inline Image::Image(const Image& img)
    : Image(img.width, img.height)
{
    LOOP_IMAGE
    {
        data[x][y] = img.data[x][y];
    }
}

inline Image::Image(Image&& img)
    : width(img.width)
    , height(img.height)
    , data(img.data)
{
    img.data = nullptr;
    img.width = 0;
    img.height = 0;
}

inline Image::Image(std::string path)
{
    int channels = 0;
    stbi_set_flip_vertically_on_load(true);
    const unsigned char* stb_data = stbi_load(path.c_str(), &width, &height, &channels, CHANNELS);

    if (!channels)
    {
        data = nullptr;
        std::cout << "[ERROR] Could not load image at " << path << "\n";
        return;
    }

    data = new RGBA*[width];
    for (int x = 0; x < width; x++)
    {
        data[x] = new RGBA[height]{};
    }

    uint64_t index = 0;

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            RGBA& rgba = data[x][y];

            rgba.R = stb_data[index++];
            rgba.G = stb_data[index++];
            rgba.B = stb_data[index++];
            rgba.A = stb_data[index++];
        }
    }

    STBI_FREE((void*)stb_data);
}

inline Image::Image(int width_in, int height_in)
    : width(std::max(1, width_in))
    , height(std::max(1, height_in))
{
    data = new RGBA*[width];

    for (int x = 0; x < width; x++)
    {
        data[x] = new RGBA[height]{};
    }
}

inline Image::Image(const void* data_in, int width_in, int height_in, bool flip_vertical)
    : Image(width_in, height_in)
{
    int index = 0;
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            RGBA rgba;
            rgba.R = static_cast<const char*>(data_in)[index++];
            rgba.G = static_cast<const char*>(data_in)[index++];
            rgba.B = static_cast<const char*>(data_in)[index++];

            int y_adj = (flip_vertical) ? height - y - 1 : y;
            data[x][y_adj] = rgba;
        }
    }
}

inline Image::~Image()
{
    freeData();
}

inline Image& Image::operator=(const Image& img)
{
    if (width != img.width || height != img.height)
    {
        *this = Image(img.width, img.height);
    }

    LOOP_IMAGE
    {
        data[x][y] = img.data[x][y];;
    }

    return *this;
}

inline Image& Image::operator=(Image&& img)
{
    if (this != &img)
    {
        freeData();
        data = img.data;
        width = img.width;
        height = img.height;

        img.data = nullptr;
        img.width = 0;
        img.height = 0;
    }
    return *this;
}

inline int Image::getWidth()  const noexcept { return width; }
inline int Image::getHeight() const noexcept { return height; }

inline bool Image::containsCoord(int x, int y) const noexcept
{
    return x >= 0 && y >= 0 && x < width && y < height;
}

inline bool Image::containsCoord(double x, double y) const noexcept
{
    return x >= 0 && y >= 0 && x < width && y < height;
}

inline RGBA* Image::operator[] (int x) noexcept
{
    return data[x];
}

inline const RGBA* Image::operator[](int x) const noexcept
{
    return data[x];
}

inline RGBA& Image::at_safe(int x, int y) noexcept
{
    x = std::min(std::max(x, 0), width-1);
    y = std::min(std::max(y, 0), height-1);

    return data[x][y];
}

inline const RGBA& Image::at_safe(int x, int y) const noexcept
{
    x = std::min(std::max(x, 0), width-1);
    y = std::min(std::max(y, 0), height-1);

    return data[x][y];
}

inline RGBA& Image::at(int x, int y)
{
    if (containsCoord(x, y))
        return data[x][y];
    else
        throw std::runtime_error("Out of bounds Image access!");
}

inline const RGBA& Image::at(int x, int y) const
{
    if (containsCoord(x, y))
        return data[x][y];
    else
        throw std::runtime_error("Out of bounds Image access!");
}

inline RGBA Image::sample(double x, double y, math::EaseCurveRGBA sampler) const
{
    if (!containsCoord(x, y))
    {
        throw std::runtime_error("Out of bounds Image access!");
    }

    constexpr double adj = 0.4714045;

    int x_0 = std::floor(std::max(0.0, x - adj));
    int y_0 = std::floor(std::max(0.0, y - adj));
    int x_1 = std::min(x_0+1, width - 1);
    int y_1 = std::min(y_0+1, height - 1);

    const RGBA& c00 = data[x_0][y_0];
    const RGBA& c01 = data[x_0][y_1];
    const RGBA& c10 = data[x_1][y_0];
    const RGBA& c11 = data[x_1][y_1];

    double x_itl = std::max(0.0, x - adj - x_0);
    double y_itl = std::max(0.0, y - adj - y_0);

    // blur together
    RGBA c_y0 = sampler(c00, c10, x_itl);
    RGBA c_y1 = sampler(c01, c11, x_itl);
    return sampler(c_y0, c_y1, y_itl);
}

inline void Image::resize(int newWidth, int newHeight, math::EaseCurveRGBA sampler)
{
    if (newWidth < 1 || newHeight < 1)
    {
        throw std::runtime_error("Can't create an image with zero or negative dimensions!");
    }

    const double scaleX = (newWidth > 1) ? (double)(width) / (double)(newWidth) : 0;
    const double scaleY = (newHeight > 1) ? (double)(height) / (double)(newHeight) : 0;

    Image img_new(newWidth, newHeight);
    for (int x = 0; x < newWidth; x++)
    {
        for (int y = 0; y < newHeight; y++)
        {
            if (sampler)
                img_new.data[x][y] = sample(x*scaleX, y*scaleY, sampler);
            else
                img_new.data[x][y] = at(x*scaleX, y*scaleY);
        }
    }

    *this = img_new;
}

inline void Image::fillColor(int x1, int y1, int x2, int y2, RGBA color) noexcept
{
    int xStep = ((x2 - x1) > 0) - ((x2 - x1) < 0);
    int yStep = ((y2 - y1) > 0) - ((y2 - y1) < 0);

    for (int x = x1; x < x2; x += xStep)
    {
        if (x < 0 || x >= width) continue;

        for (int y = y1; y < y2; y += yStep)
        {
            if (y < 0 || y >= height) continue;

            data[x][y] = color;
        }
    }
}

inline void Image::makeTransparent(bool(*condition)(const RGBA& rgba))
{
    LOOP_IMAGE
    {
        RGBA& rgba = data[x][y];
        if (condition(rgba))
        {
            rgba.A = 0;
        }
    }
}

inline void Image::setAlpha(unsigned char a)
{
    LOOP_IMAGE
    {
        data[x][y].A = a;
    }
}

inline bool Image::save(std::string path, filetype type) const
{
    unsigned int rawDataSize = width * height * sizeof(char) * CHANNELS;
    unsigned char* rawData = new unsigned char[rawDataSize];

    int index = 0;
    for (int y = height-1; y >= 0; y--)
    {
        for (int x = 0; x < width; x++)
        {
            const RGBA& col = data[x][y];

            rawData[index++] = col.R;
            rawData[index++] = col.G;
            rawData[index++] = col.B;
            rawData[index++] = col.A;
        }
    }

    int result = 0;
    switch (type)
    {
    case filetype::PNG:
        result = stbi_write_png(path.c_str(), width, height, CHANNELS, rawData, width*CHANNELS);
        break;
    case filetype::JPG:
        result = stbi_write_jpg(path.c_str(), width, height, CHANNELS, rawData, 100);
        break;
    default:
        std::cout << "Unrecognized filetype: " << (int)type << "\n";
    }

    delete[] rawData;

    if (result)
    {
        std::cout << "Image written to " << path << "\n";
        return true;
    }
    else
    {
        std::cout << "[ERROR] Could not write image to " << path << "\n";
        return false;
    }
}

inline void Image::writeBinary(std::ostream& os, bool byRow) const
{
    if (byRow)
    {
        for (int y = height - 1; y >= 0; y--)
        {
            for (int x = 0; x < width; x++)
            {
                os.write((const char*)&(data[x][y].R), 1);
                os.write((const char*)&(data[x][y].G), 1);
                os.write((const char*)&(data[x][y].B), 1);
            }
        }
    }
    else
    {
        LOOP_IMAGE
        {
            os.write((const char*)&(data[x][y].R), 1);
            os.write((const char*)&(data[x][y].G), 1);
            os.write((const char*)&(data[x][y].B), 1);
        }
    }
}

inline void Image::freeData()
{
    if (data)
    {
        for (int x = 0; x < width; x++)
        {
            delete[] data[x];
        }
        delete[] data;

        data = nullptr;
    }
}

#undef LOOP_IMAGE

} // namespace zimg