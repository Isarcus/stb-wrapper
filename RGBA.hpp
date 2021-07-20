#ifndef ISARCUS_RGBA_HPP
#define ISARCUS_RGBA_HPP

namespace zimg
{

typedef struct RGBA {
    RGBA();
    RGBA(unsigned char r, unsigned char g, unsigned char b, unsigned char a = 255);

    int diff(RGBA compare) const;
    double brightness() const; // scale of 0 to 1

    bool operator==(RGBA rgba);
    bool operator!=(RGBA rgba);

    unsigned char R, G, B, A;
} RGBA;

}

#include "impl/RGBA.ipp"

#endif