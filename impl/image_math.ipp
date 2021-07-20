#include <cmath>

#ifndef ISARCUS_IMAGE_MATH_HPP
#include "../image_math.hpp"
#endif

namespace zimg
{

namespace math
{

// Basic linear interpolation
inline double interpLinear(double val0, double val1, double t)
{
    return val0 + ((val1 - val0) * t);
}

inline RGBA interpLinear(RGBA c0, RGBA c1, double t)
{
    return RGBA(
        std::round(interpLinear(c0.R, c1.R, t)),
        std::round(interpLinear(c0.G, c1.G, t)),
        std::round(interpLinear(c0.B, c1.B, t)),
        std::round(interpLinear(c0.A, c1.A, t))
    );
}

// Derivative maxes at 1.875; symmetrical
inline double interpEase5(double t)
{
    // 6t^5 - 15t^4 + 10t^3
    return std::pow(t, 3) * (6.0*std::pow(t, 2) - 15.0*t + 10.0);
}

inline double interpEase5(double val0, double val1, double t)
{
    return interpLinear(val0, val1, interpEase5(t));
}

inline RGBA interpEase5(RGBA c0, RGBA c1, double t)
{
    return interpLinear(c0, c1, interpEase5(t));
}

// Derivative maxes at 5.0; symmetrical
inline double interpBezier5(double t)
{
    if (t < 0.5)
    {
        return 16.0*std::pow(t, 5);
    }
    else
    {
        return 1.0 - 16.0*std::pow(1.0 - t, 5);
    }
}

inline double interpBezier5(double val0, double val1, double t)
{
    return interpLinear(val0, val1, interpBezier5(t));
}

inline RGBA interpBezier5(RGBA c0, RGBA c1, double t)
{
    return interpLinear(c0, c1, interpBezier5(t));
}

} // namespace math

} // namespace zimg