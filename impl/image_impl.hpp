/*
* Created by iabrooks on May 3, 2021
*
* This file contains code for the easy creation and saving of PNG images
* using the publicly available "STB Image" library.
*/

#ifndef ISARCUS_IMAGE_HPP
#include "../image.hpp"
#endif

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#include <cmath>
#include <iostream>
#include <fstream>

#define LOOP_IMAGE for (int x = 0; x < width; x++) for (int y = 0; y < height; y++)

namespace ISARCUS_IMAGE_NAMESPACE
{

constexpr double PI = 3.141592653589793238462643383279502884197169399375105;

//      //
// RGBA //
//      //

RGBA::RGBA()
    : R(0)
    , G(0)
    , B(0)
    , A(255)
{}

RGBA::RGBA(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
    : R(r)
    , G(g)
    , B(b)
    , A(a)
{}

bool RGBA::operator==(RGBA rgba)
{
    return R == rgba.R &&
           G == rgba.G &&
           B == rgba.B &&
           A == rgba.A;
}

bool RGBA::operator!=(RGBA rgba)
{
    return !((*this) == rgba);
}

int RGBA::diff(RGBA compare) const
{
    int sum = 0;
    sum += std::abs((int)R - (int)compare.R);
    sum += std::abs((int)G - (int)compare.G);
    sum += std::abs((int)B - (int)compare.B);
    return sum;
}

double RGBA::brightness() const
{
    return ((double)R + (double)G + (double)B) / (765.0) * ((double)A / 255.0);
}

//       //
// Image //
//       //

Image::Image(const Image& img)
    : Image(img.width, img.height)
{
    LOOP_IMAGE
    {
        data[x][y] = img.data[x][y];
    }
}

Image::Image(Image&& img)
    : data(img.data)
{
    img.data = nullptr;
    img.width = 0;
    img.height = 0;
}

Image::Image(std::string path)
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

Image::Image(int width, int height)
    : width(width)
    , height(height)
{
    data = new RGBA*[width];

    for (int x = 0; x < width; x++)
    {
        data[x] = new RGBA[height]{};
    }
}

Image::~Image()
{
    freeData();
}

Image& Image::operator=(const Image& img)
{
    if (width != img.width || height != img.height)
    {
        freeData();
        *this = Image(img.width, img.height);
    }

    LOOP_IMAGE
    {
        data[x][y] = img.data[x][y];;
    }

    return *this;
}

Image& Image::operator=(Image&& img)
{
    freeData();
    data = img.data;
    width = img.width;
    height = img.height;

    img.data = nullptr;
    img.width = 0;
    img.height = 0;

    return *this;
}

int Image::getWidth()  const noexcept { return width; }
int Image::getHeight() const noexcept { return height; }

bool Image::containsCoord(int x, int y) const noexcept
{
    return x >= 0 && y >= 0 && x < width && y < height;
}

RGBA* Image::operator[] (int x) noexcept
{
    return data[x];
}

const RGBA* Image::operator[](int x) const noexcept
{
    return data[x];
}

RGBA& Image::at_safe(int x, int y) noexcept
{
    x = std::min(std::max(x, 0), width-1);
    y = std::min(std::max(y, 0), height-1);

    return data[x][y];
}

const RGBA& Image::at_safe(int x, int y) const noexcept
{
    x = std::min(std::max(x, 0), width-1);
    y = std::min(std::max(y, 0), height-1);

    return data[x][y];
}

RGBA& Image::at(int x, int y)
{
    if (containsCoord(x, y))
        return data[x][y];
    else
        throw std::runtime_error("Out of bounds Image access!");
}

const RGBA& Image::at(int x, int y) const
{
    if (containsCoord(x, y))
        return data[x][y];
    else
        throw std::runtime_error("Out of bounds Image access!");
}

void Image::fillColor(int x1, int y1, int x2, int y2, RGBA color) noexcept
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

void Image::makeTransparent(bool(*condition)(const RGBA& rgba))
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

void Image::setAlpha(unsigned char a)
{
    LOOP_IMAGE
    {
        data[x][y].A = a;
    }
}

void Image::save(std::string path, filetype type)
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

    switch (type)
    {
    case filetype::PNG:
        stbi_write_png(path.c_str(), width, height, CHANNELS, rawData, width*CHANNELS);
        break;
    case filetype::JPG:
        stbi_write_jpg(path.c_str(), width, height, CHANNELS, rawData, 100);
        break;
    default:
        std::cout << "Unrecognized filetype: " << (int)type << "\n";
    }

    delete[] rawData;

    std::cout << "Image written to " << path << "\n";
}

void Image::freeData()
{
    if (data)
    {
        for (int x = 0; x < width; x++)
        {
            delete[] data[x];
        }
        delete[] data;
    }
}

#undef STB_IMAGE_IMPLEMENTATION
#undef STB_IMAGE_WRITE_IMPLEMENTATION
#undef LOOP_IMAGE

} // namespace ISARCUS_IMAGE_NAMESPACE