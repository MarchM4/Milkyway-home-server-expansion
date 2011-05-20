/* Copyright 2010 Matthew Arsenault, Travis Desell, Boleslaw
Szymanski, Heidi Newberg, Carlos Varela, Malik Magdon-Ismail and
Rensselaer Polytechnic Institute.

This file is part of Milkway@Home.

Milkyway@Home is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Milkyway@Home is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Milkyway@Home.  If not, see <http://www.gnu.org/licenses/>.
*/

#if !defined (_MILKYWAY_MATH_H_INSIDE_) && !defined(MILKYWAY_MATH_COMPILATION)
#error "Only milkyway_math.h can be included directly."
#endif

#ifndef _MILKYWAY_MATH_FUNCTIONS_H_
#define _MILKYWAY_MATH_FUNCTIONS_H_

/* simple math macros */
#define cube(x) ((x) * (x) * (x))
#define sqr(x)  ((x) * (x))
#define inv(x)  ((real) 1.0 / (x))

#if ENABLE_CRLIBM && DOUBLEPREC
  #define mw_sin   sin_rn
  #define mw_cos   cos_rn
  #define mw_tan   tan_rn
  #define mw_asin  asin_rn
  #define mw_acos  acos_rn
  #define mw_atan  atan_rn

  #define mw_log   log_rn
  #define mw_log1p log1p_rn
  #define mw_exp   exp_rn
  #define mw_expm1 expm1_rn
  #define mw_log10 log10_rn
  #define mw_sinh  sinh_rn
  #define mw_cosh  cosh_rn
  #define mw_tanh  tanh_rn
  #define mw_pow   pow_rn
#else
  #define mw_sin   sin
  #define mw_cos   cos
  #define mw_tan   tan
  #define mw_asin  asin
  #define mw_acos  acos
  #define mw_atan  atan
  #define mw_log   log
  #define mw_log1p log1p
  #define mw_exp   exp
  #define mw_expm1 expm1
  #define mw_log10 log10
  #define mw_sinh  sinh
  #define mw_cosh  cosh
  #define mw_tanh  tanh
  #define mw_pow   pow
#endif /* ENABLE_CRLIBM */

#define mw_abs   fabs
#define mw_max   fmax
#define mw_min   fmin

#define mw_acosh acosh
#define mw_acospi(x) (mw_acos(x) / M_PI))
#define mw_asinh asinh
#define mw_asinpi asinpi (mw_asin(x) / M_PI))
#define mw_atan2 atan2
#define mw_atanh atanh
#define mw_atanpi(x) (mw_atan(x) / M_PI)
#define mw_atan2pi(x) (mw_atan2(x) / M_PI)
#define mw_cbrt cbrt
#define mw_ceil ceil
#define mw_copysign copysign
#define mw_cospi(x) mw_cos(M_PI * (x))
#define mw_sinpi(x) mw_sin(M_PI * (x))
#define mw_tanpi(x) (mw_tan(M_PI * (x)))
#define mw_erfc erfc
#define mw_erf erf

#if HAVE_EXP2
  #define mw_exp2 exp2
#else
  #define mw_exp2(x) mw_pow(2.0, (x))
#endif /* HAVE_EXP2 */

#if HAVE_EXP10
  #define mw_exp10 exp10
#else
  #define mw_exp10(x) mw_powr((real) 10.0, x)
#endif /* HAVE_EXP10 */

#define mw_fabs fabs
#define mw_fdim fdim
#define mw_floor floor

/* TODO: Have fma */
#define mw_fma(a, b, c) (((a) * (b)) + (c))

#define mw_fmax fmax
#define mw_fmin fmin
#define mw_fmod fmod

/* CHECKME: mw_fract */
#define mw_fract(x) mw_fmin((x) – mw_floor(x), 0x1.fffffep-1f)

#define mw_frexp frexp
#define mw_hypot(x, y) mw_sqrt(sqr(x) + sqr(y))
#define mw_ilogb ilogb
#define mw_ldexp ldexp
#define mw_tgamma tgamma
#define mw_tgamma_r tgamma_r
#define mw_lgamma lgamma
#define mw_lgamma_r lgamma_r
#define mw_log2 log2
#define mw_logb logb
#define mw_mad(a, b, c) (((a) * (b)) + (c))
#define mw_modf modf
#define mw_nan nan
#define mw_nextafter nextafter

/* TODO: assertions that these satisfy integer y or x >= 0 */
#define mw_pown(x, iy) pow(x, iy)
#define mw_powr(x, y) pow(x, y)

#define mw_remainder remainder
#define mw_remquo remquo
#define mw_rint rint
#define mw_rootn(x, y) mw_pow((x), 1.0 / (y))
#define mw_round round

#if HAVE_RSQRT && USE_RSQRT
  /* warning: This loses precision */
  #define mw_rsqrt rsqrt
#else
  #define mw_rsqrt(x) (1.0 / mw_sqrt(x))
#endif /* HAVE_RSQRT && USE_RSQRT */


#if HAVE_SINCOS
  /* Assuming glibc style, e.g. sincos(x, &sinval, &cosval) */
  #define mw_sincos sincos
#else
    /* TODO: Using real sincos() would be nice in coordinate conversions
   for speed, but it's a glibc extension. It is in opencl though.  We
   could take it from glibc, but it's in assembly and would be kind of
   annoying to add, but probably worth it. */
  #define mw_sincos(x, s, c) { *(s) = mw_sin(x); *(c) = mw_cos(x); }
#endif /* HAVE_SINCOS */

#define mw_sqrt sqrt
#define mw_tgamma tgamma
#define mw_trunc trunc


#endif /* _MILKYWAY_MATH_FUNCTIONS_H_ */

