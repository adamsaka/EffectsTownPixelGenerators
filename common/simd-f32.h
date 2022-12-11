/********************************************************************************************************

Authors:		(c) 2022 Maths Town

Licence:		The MIT License

*********************************************************************************************************
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
associated documentation files (the "Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the
following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial
portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
********************************************************************************************************

Basic SIMD Types for 32-bit floats.

Each type provides the following operations so they can be used in templated functions.

Support information.
	static bool cpu_supported(CpuInformation cpuid)
	static constexpr int size_of_element()
	static constexpr int number_of_elements()

Operators (rhs same type):


Operators (rhs int)



*********************************************************************************************************/
#pragma once

#include <immintrin.h>

#include "simd-cpuid.h"
#include <cmath>

/**************************************************************************************************
 * SIMD 512 type.  Contains 16 x 32bit Floats
 * Requires AVX-512F support.
 * ************************************************************************************************/
struct Simd512Float32 {
	__m512 v;

	Simd512Float32(__m512 a) : v(a) {};

	//*****Support Informtion*****
	static bool cpu_supported(CpuInformation cpuid) {
		return cpuid.has_avx512_f() ;
	}
	static constexpr int size_of_element() { return sizeof(float); }
	static constexpr int number_of_elements() { return 16; }

	//*****Addition Operators*****
	Simd512Float32& operator+=(const Simd512Float32& rhs) noexcept { v = _mm512_add_ps(v, rhs.v); return *this; }
	Simd512Float32& operator+=(float rhs) noexcept { v = _mm512_add_ps(v, _mm512_set1_ps(rhs));	return *this; }

	//*****Subtraction Operators*****
	Simd512Float32& operator-=(const Simd512Float32& rhs) noexcept { v = _mm512_sub_ps(v, rhs.v); return *this; }
	Simd512Float32& operator-=(float rhs) noexcept { v = _mm512_sub_ps(v, _mm512_set1_ps(rhs));	return *this; }

	//*****Multiplication Operators*****
	Simd512Float32& operator*=(const Simd512Float32& rhs) noexcept { v = _mm512_mul_ps(v, rhs.v); return *this; }
	Simd512Float32& operator*=(float rhs) noexcept { v = _mm512_mul_ps(v, _mm512_set1_ps(rhs)); return *this; }

	//*****Division Operators*****
	Simd512Float32& operator/=(const Simd512Float32& rhs) noexcept { v = _mm512_div_ps(v, rhs.v); return *this; }
	Simd512Float32& operator/=(float rhs) noexcept { v = _mm512_div_ps(v, _mm512_set1_ps(rhs));	return *this; }

};

//*****Addition Operators*****
inline Simd512Float32 operator+(Simd512Float32  lhs, const Simd512Float32& rhs) noexcept { lhs += rhs; return lhs; }
inline Simd512Float32 operator+(Simd512Float32  lhs, float rhs) noexcept { lhs += rhs; return lhs; }
inline Simd512Float32 operator+(float lhs, Simd512Float32 rhs) noexcept { rhs += lhs; return rhs; }

//*****Subtraction Operators*****
inline Simd512Float32 operator-(Simd512Float32  lhs, const Simd512Float32& rhs) noexcept { lhs -= rhs; return lhs; }
inline Simd512Float32 operator-(Simd512Float32  lhs, float rhs) noexcept { lhs -= rhs; return lhs; }
inline Simd512Float32 operator-(const float lhs, const Simd512Float32& rhs) noexcept { return Simd512Float32(_mm512_sub_ps(_mm512_set1_ps(lhs), rhs.v)); }

//*****Multiplication Operators*****
inline Simd512Float32 operator*(Simd512Float32  lhs, const Simd512Float32& rhs) noexcept { lhs *= rhs; return lhs; }
inline Simd512Float32 operator*(Simd512Float32  lhs, float rhs) noexcept { lhs *= rhs; return lhs; }
inline Simd512Float32 operator*(float lhs, Simd512Float32 rhs) noexcept { rhs *= lhs; return rhs; }

//*****Division Operators*****
inline Simd512Float32 operator/(Simd512Float32  lhs, const Simd512Float32& rhs) noexcept { lhs /= rhs;	return lhs; }
inline Simd512Float32 operator/(Simd512Float32  lhs, float rhs) noexcept { lhs /= rhs; return lhs; }
inline Simd512Float32 operator/(const float lhs, const Simd512Float32& rhs) noexcept { return Simd512Float32(_mm512_div_ps(_mm512_set1_ps(lhs), rhs.v)); }

//*****Rounding Functions*****
inline Simd512Float32 floor(Simd512Float32 a) { return  Simd512Float32(_mm512_floor_ps(a.v)); }
inline Simd512Float32 ceil(Simd512Float32 a)  { return  Simd512Float32(_mm512_ceil_ps(a.v)); }
inline Simd512Float32 trunc(Simd512Float32 a) { return  Simd512Float32(_mm512_trunc_ps(a.v)); }
inline Simd512Float32 round(Simd512Float32 a) { return  Simd512Float32(_mm512_roundscale_ps(a.v, _MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC)); }
inline Simd512Float32 fract(Simd512Float32 a) { return a - floor(a); }

//*****Min/Max*****
inline Simd512Float32 min(Simd512Float32 a, Simd512Float32 b) { return Simd512Float32(_mm512_min_ps(a.v, b.v)); }
inline Simd512Float32 max(Simd512Float32 a, Simd512Float32 b) { return Simd512Float32(_mm512_max_ps(a.v, b.v)); }


//*****Approximate Functions*****
inline Simd512Float32 reciprocal_approx(Simd512Float32 a) { return Simd512Float32(_mm512_rcp14_ps(a.v)); }


/**************************************************************************************************
 * SIMD 256 type.  Contains 8 x 32bit Floats
 * Requires AVX support.
 * ************************************************************************************************/
struct Simd256Float32 {
	__m256 v;

	Simd256Float32(__m256 a) : v(a) {};

	//*****Support Informtion*****
	static bool cpu_supported(CpuInformation cpuid) {
		return cpuid.has_avx();
	}
	static constexpr int size_of_element() { return sizeof(float); }
	static constexpr int number_of_elements() { return 8; }

	//*****Addition Operators*****
	Simd256Float32& operator+=(const Simd256Float32& rhs) noexcept { v = _mm256_add_ps(v, rhs.v); return *this; }
	Simd256Float32& operator+=(float rhs) noexcept { v = _mm256_add_ps(v, _mm256_set1_ps(rhs));	return *this; }

	//*****Subtraction Operators*****
	Simd256Float32& operator-=(const Simd256Float32& rhs) noexcept { v = _mm256_sub_ps(v, rhs.v); return *this; }
	Simd256Float32& operator-=(float rhs) noexcept { v = _mm256_sub_ps(v, _mm256_set1_ps(rhs));	return *this; }

	//*****Multiplication Operators*****
	Simd256Float32& operator*=(const Simd256Float32& rhs) noexcept { v = _mm256_mul_ps(v, rhs.v); return *this; }
	Simd256Float32& operator*=(float rhs) noexcept { v = _mm256_mul_ps(v, _mm256_set1_ps(rhs)); return *this; }

	//*****Division Operators*****
	Simd256Float32& operator/=(const Simd256Float32& rhs) noexcept { v = _mm256_div_ps(v, rhs.v); return *this; }
	Simd256Float32& operator/=(float rhs) noexcept { v = _mm256_div_ps(v, _mm256_set1_ps(rhs));	return *this; }

};

//*****Addition Operators*****
inline Simd256Float32 operator+(Simd256Float32  lhs, const Simd256Float32& rhs) noexcept { lhs += rhs; return lhs; }
inline Simd256Float32 operator+(Simd256Float32  lhs, float rhs) noexcept { lhs += rhs; return lhs; }
inline Simd256Float32 operator+(float lhs, Simd256Float32 rhs) noexcept { rhs += lhs; return rhs; }

//*****Subtraction Operators*****
inline Simd256Float32 operator-(Simd256Float32  lhs, const Simd256Float32& rhs) noexcept { lhs -= rhs; return lhs; }
inline Simd256Float32 operator-(Simd256Float32  lhs, float rhs) noexcept { lhs -= rhs; return lhs; }
inline Simd256Float32 operator-(const float lhs, const Simd256Float32& rhs) noexcept { return Simd256Float32(_mm256_sub_ps(_mm256_set1_ps(lhs), rhs.v)); }

//*****Multiplication Operators*****
inline Simd256Float32 operator*(Simd256Float32  lhs, const Simd256Float32& rhs) noexcept { lhs *= rhs; return lhs; }
inline Simd256Float32 operator*(Simd256Float32  lhs, float rhs) noexcept { lhs *= rhs; return lhs; }
inline Simd256Float32 operator*(float lhs, Simd256Float32 rhs) noexcept { rhs *= lhs; return rhs; }

//*****Division Operators*****
inline Simd256Float32 operator/(Simd256Float32  lhs, const Simd256Float32& rhs) noexcept { lhs /= rhs;	return lhs; }
inline Simd256Float32 operator/(Simd256Float32  lhs, float rhs) noexcept { lhs /= rhs; return lhs; }
inline Simd256Float32 operator/(const float lhs, const Simd256Float32& rhs) noexcept { return Simd256Float32(_mm256_div_ps(_mm256_set1_ps(lhs), rhs.v)); }

//*****Rounding Functions*****
inline Simd256Float32 floor(Simd256Float32 a) { return  Simd256Float32(_mm256_floor_ps(a.v)); }
inline Simd256Float32 ceil(Simd256Float32 a) { return  Simd256Float32(_mm256_ceil_ps(a.v)); }
inline Simd256Float32 trunc(Simd256Float32 a) { return  Simd256Float32(_mm256_trunc_ps(a.v)); }
inline Simd256Float32 round(Simd256Float32 a) { return  Simd256Float32(_mm256_round_ps(a.v, _MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC)); }
inline Simd256Float32 fract(Simd256Float32 a) { return a - floor(a); }

//*****Min/Max*****
inline Simd256Float32 min(Simd256Float32 a, Simd256Float32 b) { return Simd256Float32(_mm256_min_ps(a.v, b.v)); }
inline Simd256Float32 max(Simd256Float32 a, Simd256Float32 b) { return Simd256Float32(_mm256_max_ps(a.v, b.v)); }

//*****Approximate Functions*****
inline Simd256Float32 reciprocal_approx(Simd256Float32 a) { return Simd256Float32(_mm256_rcp_ps(a.v)); }



/**************************************************************************************************
 * SIMD 256 type.  Contains 16 x 32bit Floats
 * Requires AVX support.
 * ************************************************************************************************/
struct FallbackFloat32 {
	float v;

	FallbackFloat32(float a) : v(a) {};

	//*****Support Informtion*****
	static bool cpu_supported(CpuInformation cpuid) {
		return true;
	}
	static constexpr int size_of_element() { return sizeof(float); }
	static constexpr int number_of_elements() { return 1; }

	//*****Addition Operators*****
	FallbackFloat32& operator+=(const FallbackFloat32& rhs) noexcept { v += rhs.v; return *this; }
	FallbackFloat32& operator+=(float rhs) noexcept { v += rhs;	return *this; }

	//*****Subtraction Operators*****
	FallbackFloat32& operator-=(const FallbackFloat32& rhs) noexcept { v -= rhs.v; return *this; }
	FallbackFloat32& operator-=(float rhs) noexcept { v -= rhs;	return *this; }

	//*****Multiplication Operators*****
	FallbackFloat32& operator*=(const FallbackFloat32& rhs) noexcept { v *= rhs.v; return *this; }
	FallbackFloat32& operator*=(float rhs) noexcept { v *= rhs; return *this; }

	//*****Division Operators*****
	FallbackFloat32& operator/=(const FallbackFloat32& rhs) noexcept { v /= rhs.v; return *this; }
	FallbackFloat32& operator/=(float rhs) noexcept { v /= rhs;	return *this; }

};

//*****Addition Operators*****
inline FallbackFloat32 operator+(FallbackFloat32  lhs, const FallbackFloat32& rhs) noexcept { lhs += rhs; return lhs; }
inline FallbackFloat32 operator+(FallbackFloat32  lhs, float rhs) noexcept { lhs += rhs; return lhs; }
inline FallbackFloat32 operator+(float lhs, FallbackFloat32 rhs) noexcept { rhs += lhs; return rhs; }

//*****Subtraction Operators*****
inline FallbackFloat32 operator-(FallbackFloat32  lhs, const FallbackFloat32& rhs) noexcept { lhs -= rhs; return lhs; }
inline FallbackFloat32 operator-(FallbackFloat32  lhs, float rhs) noexcept { lhs -= rhs; return lhs; }
inline FallbackFloat32 operator-(const float lhs, const FallbackFloat32& rhs) noexcept { return FallbackFloat32(lhs-rhs.v); }

//*****Multiplication Operators*****
inline FallbackFloat32 operator*(FallbackFloat32  lhs, const FallbackFloat32& rhs) noexcept { lhs *= rhs; return lhs; }
inline FallbackFloat32 operator*(FallbackFloat32  lhs, float rhs) noexcept { lhs *= rhs; return lhs; }
inline FallbackFloat32 operator*(float lhs, FallbackFloat32 rhs) noexcept { rhs *= lhs; return rhs; }

//*****Division Operators*****
inline FallbackFloat32 operator/(FallbackFloat32  lhs, const FallbackFloat32& rhs) noexcept { lhs /= rhs;	return lhs; }
inline FallbackFloat32 operator/(FallbackFloat32  lhs, float rhs) noexcept { lhs /= rhs; return lhs; }
inline FallbackFloat32 operator/(const float lhs, const FallbackFloat32& rhs) noexcept { return FallbackFloat32(lhs-rhs.v); }

//*****Rounding Functions*****
inline FallbackFloat32 floor(FallbackFloat32 a) { return  FallbackFloat32(std::floor(a.v)); }
inline FallbackFloat32 ceil(FallbackFloat32 a) { return  FallbackFloat32(std::ceil(a.v)); }
inline FallbackFloat32 trunc(FallbackFloat32 a) { return  FallbackFloat32(std::trunc(a.v)); }
inline FallbackFloat32 round(FallbackFloat32 a) { return  FallbackFloat32(std::round(a.v)); }
inline FallbackFloat32 fract(FallbackFloat32 a) { return a - floor(a); }


//*****Min/Max*****
inline FallbackFloat32 min(FallbackFloat32 a, FallbackFloat32 b) { return FallbackFloat32(std::min(a.v, b.v)); }
inline FallbackFloat32 max(FallbackFloat32 a, FallbackFloat32 b) { return FallbackFloat32(std::max(a.v, b.v)); }


//*****Approximate Functions*****
inline FallbackFloat32 reciprocal_approx(FallbackFloat32 a) { return FallbackFloat32(1.0f/a.v); }