/*
* Created by Isarcus on May July 19, 2021
*
* This file contains code for the easy creation and saving of images
* using the publicly available "STB Image" library.
*/

#ifndef ISARCUS_IMAGE_HPP
#define ISARCUS_IMAGE_HPP

#ifndef ISARCUS_IMAGE_NAMESPACE
#define ISARCUS_IMAGE_NAMESPACE zimg
#endif

#include <string>

namespace ISARCUS_IMAGE_NAMESPACE
{

enum class filetype
{
    PNG,
    JPG,
};

typedef struct RGBA {
    RGBA();
    RGBA(unsigned char r, unsigned char g, unsigned char b, unsigned char a = 255);

    int diff(RGBA compare) const;
    double brightness() const; // scale of 0 to 1

    bool operator==(RGBA rgba);
    bool operator!=(RGBA rgba);

    unsigned char R, G, B, A;
} RGBA;


class Image {
public:
    Image(const Image& img);
    Image(Image&& img);
    Image(std::string path);
    Image(int width, int height);
    ~Image();

    Image& operator=(const Image& img);
    Image& operator=(Image&& img);

    // Characteristics //

    int getWidth() const noexcept;
    int getHeight() const noexcept;
    bool containsCoord(int x, int y) const noexcept;
    
    // Accessors //

    RGBA* operator[](int x) noexcept;
    const RGBA* operator[](int x) const noexcept;
    RGBA& at_safe(int x, int y) noexcept;
    const RGBA& at_safe(int x, int y) const noexcept;
    RGBA& at(int x, int y);
    const RGBA& at(int x, int y) const;

    // Modifiers //

    void fillColor(int x1, int y1, int x2, int y2, RGBA color) noexcept;
    void makeTransparent(bool(*condition)(const RGBA& rgba));
    void setAlpha(unsigned char a);

    // Save image at a given path, with a desired format (default PNG)
    void save(std::string path, filetype type = filetype::PNG);

private:
    int width, height;
    RGBA** data;

    static const int CHANNELS = 4;

    void freeData();
};

} // namespace ISARCUS_IMAGE_NAMESPACE

#ifdef ISARCUS_IMAGE_IMPLEMENTATION
#include "impl/image_impl.hpp"
#endif

#endif // ifndef ISARCUS_IMAGE_HPP