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
    Image(int width_in, int height_in);
    Image(const void* data_in, int width_in, int height_in, bool flip_vertical = false); // expects char rgb encoding, *for now*
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
    RGBA sample(double x, double y, math::EaseCurveRGBA sampler=math::interpEase5) const;

    // Modifiers //

    void resize(int newWidth, int newHeight, math::EaseCurveRGBA sampler = nullptr);
    void fillColor(int x1, int y1, int x2, int y2, RGBA color) noexcept;
    void makeTransparent(bool(*condition)(const RGBA& rgba));
    void setAlpha(unsigned char a);

    // Save image at a given path, with a desired format (default PNG)
    // Returns true upon successful save, false otherwise
    bool save(std::string path, filetype type = filetype::PNG) const;

    // Writes image as binary data (TODO: add encoding parameter, e.g. "rgb8" or "bgr16")
    void writeBinary(std::ostream& os, bool byRow = true) const;

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

#include "impl/Image.ipp"

#endif // ifndef ISARCUS_IMAGE_HPP