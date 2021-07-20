#ifndef ISARCUS_IMAGE_MATH_HPP
#define ISARCUS_IMAGE_MATH_HPP

#include "RGBA.hpp"

namespace zimg
{

namespace math
{

typedef RGBA (*EaseCurveRGBA)(RGBA, RGBA, double);

// Basic linear interpolation
inline double interpLinear(double val0, double val1, double t);
inline RGBA interpLinear(RGBA c0, RGBA c1, double t);

// Derivative maxes at 1.875; symmetrical
inline double interpEase5(double t);
inline double interpEase5(double val0, double val1, double t);
inline RGBA interpEase5(RGBA c0, RGBA c1, double t);

// Derivative maxes at 5.0; symmetrical
inline double interpBezier5(double t);
inline double interpBezier5(double val0, double val1, double t);
inline RGBA interpBezier5(RGBA c0, RGBA c1, double t);

} // namespace math

} // namespace zimg

#include "impl/image_math.ipp"

#endif