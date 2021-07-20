#ifndef ISARCUS_RGBA_HPP
#include "../RGBA.hpp"
#endif

#include <cmath>

namespace zimg
{

inline RGBA::RGBA()
    : R(0)
    , G(0)
    , B(0)
    , A(255)
{}

inline RGBA::RGBA(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
    : R(r)
    , G(g)
    , B(b)
    , A(a)
{}

inline bool RGBA::operator==(RGBA rgba)
{
    return R == rgba.R &&
           G == rgba.G &&
           B == rgba.B &&
           A == rgba.A;
}

inline bool RGBA::operator!=(RGBA rgba)
{
    return !((*this) == rgba);
}

inline int RGBA::diff(RGBA compare) const
{
    int sum = 0;
    sum += std::abs((int)R - (int)compare.R);
    sum += std::abs((int)G - (int)compare.G);
    sum += std::abs((int)B - (int)compare.B);
    return sum;
}

inline double RGBA::brightness() const
{
    return ((double)R + (double)G + (double)B) / (765.0) * ((double)A / 255.0);
}

} // namespace zimg