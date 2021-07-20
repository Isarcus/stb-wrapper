/*
* Created by Isarcus on May July 19, 2021
*
* This file contains code for the easy creation and saving of images
* using the publicly available "STB Image" library.
*/

#ifndef ISARCUS_IMAGE_HPP
#define ISARCUS_IMAGE_HPP

#include <string>

#include "image_math.hpp"
#include "RGBA.hpp"

namespace zimg
{

enum class filetype
{
    PNG,
    JPG,
};

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
    bool containsCoord(double x, double y) const noexcept;
    
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

    // Invariants:
    // -> data always points to a valid 2D 'rectangular' array of initialized RGBA values
    // -> width and height are always >= 0
    //
    // These will be violated if and only if an Image has been used as an rvalue reference
    // in another Image's constructor or assignment operator

    static const int CHANNELS = 4;

    void freeData();
};

} // namespace zimg

#ifdef ISARCUS_IMAGE_IMPLEMENTATION
#include "impl/Image.ipp"
#endif

#endif // ifndef ISARCUS_IMAGE_HPP