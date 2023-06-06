/********************************************************************************************************

Authors:		(c) 2023 Maths Town

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

Note: Generally CPUs have more SIMD support for floats than ints.
Always check cpu_supported() for both types when using converting/casting functions.

Each type provides the following operations so they can be used in templated functions.


Support information.
	static bool cpu_supported(CpuInformation cpuid)
	static constexpr int size_of_element()
	static constexpr int number_of_elements()

Operators (rhs same type):


Operators (rhs int)



*********************************************************************************************************/
#pragma once

#include <cmath>


#include "simd-cpuid.h"
#include "simd-concepts.h"
#include "simd-uint32.h"
#include "simd-uint64.h"

/***************************************************************************************************************************************************************************************************
 * Fallback to a single 32 bit float
 * *************************************************************************************************************************************************************************************************/
struct FallbackFloat32 {
	float v;

	typedef float F;
	typedef FallbackUInt32 U;
	typedef FallbackUInt64 U64;

	FallbackFloat32() = default;
	FallbackFloat32(float a) : v(a) {};

	
	//*****Support Informtion*****
	
	//Performs a runtime CPU check to see if this type is supported.  Checks this type ONLY (integers in same the same level may not be supported) 
	static bool cpu_supported() {return true;}

	//Performs a runtime CPU check to see if this type's microarchitecture level is supported.  (This will ensure that referernced integer types are also supported)
	static bool cpu_level_supported() {return true; }
	

#if defined(_M_X64) || defined(__x86_64)
	//Performs a runtime CPU check to see if this type is supported.  Checks this type ONLY (integers in same the same level may not be supported) 
	static bool cpu_supported(CpuInformation) {return true;}

	//Performs a runtime CPU check to see if this type's microarchitecture level is supported.  (This will ensure that referernced integer types are also supported)
	static bool cpu_level_supported(CpuInformation) {return true;}
#endif
	

	
	static constexpr int size_of_element() { return sizeof(float); }
	static constexpr int number_of_elements() { return 1; }

	//*****Access Elements*****
	F element(int)  const { return v; }
	void set_element(int, F value) { v = value; }

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

	//*****Negate Operators*****
	FallbackFloat32 operator-() const noexcept { return FallbackFloat32(-v); }

	//*****Make Functions****
	static FallbackFloat32 make_sequential(F first) { return FallbackFloat32(first); }


	static FallbackFloat32 make_from_int32(FallbackUInt32 i) { return FallbackFloat32(static_cast<float>(i.v)); }

	//*****Cast Functions****
	FallbackUInt32 bitcast_to_uint() const noexcept { return FallbackUInt32(std::bit_cast<uint32_t>(this->v)); }

	//*****Approximate Functions*****
	inline FallbackFloat32 reciprocal_approx() const noexcept { return FallbackFloat32(1.0f / v); }

};

//*****Addition Operators*****
inline FallbackFloat32 operator+(FallbackFloat32  lhs, const FallbackFloat32& rhs) noexcept { lhs += rhs; return lhs; }
inline FallbackFloat32 operator+(FallbackFloat32  lhs, float rhs) noexcept { lhs += rhs; return lhs; }
inline FallbackFloat32 operator+(float lhs, FallbackFloat32 rhs) noexcept { rhs += lhs; return rhs; }

//*****Subtraction Operators*****
inline FallbackFloat32 operator-(FallbackFloat32  lhs, const FallbackFloat32& rhs) noexcept { lhs -= rhs; return lhs; }
inline FallbackFloat32 operator-(FallbackFloat32  lhs, float rhs) noexcept { lhs -= rhs; return lhs; }
inline FallbackFloat32 operator-(const float lhs, const FallbackFloat32& rhs) noexcept { return FallbackFloat32(lhs - rhs.v); }

//*****Multiplication Operators*****
inline FallbackFloat32 operator*(FallbackFloat32  lhs, const FallbackFloat32& rhs) noexcept { lhs *= rhs; return lhs; }
inline FallbackFloat32 operator*(FallbackFloat32  lhs, float rhs) noexcept { lhs *= rhs; return lhs; }
inline FallbackFloat32 operator*(float lhs, FallbackFloat32 rhs) noexcept { rhs *= lhs; return rhs; }

//*****Division Operators*****
inline FallbackFloat32 operator/(FallbackFloat32  lhs, const FallbackFloat32& rhs) noexcept { lhs /= rhs;	return lhs; }
inline FallbackFloat32 operator/(FallbackFloat32  lhs, float rhs) noexcept { lhs /= rhs; return lhs; }
inline FallbackFloat32 operator/(const float lhs, const FallbackFloat32& rhs) noexcept { return FallbackFloat32(lhs - rhs.v); }


//*****Fused Multiply Add Fallbacks*****
// Fused Multiply Add (a*b+c)
[[nodiscard("Value calculated and not used (fma)")]]
inline FallbackFloat32 fma(const FallbackFloat32  a, const FallbackFloat32 b, const FallbackFloat32 c) { 
	//return a * b + c; 
	return std::fma(a.v, b.v, c.v);
}

// Fused Multiply Subtract (a*b-c)
[[nodiscard("Value calculated and not used (fms)")]]
inline FallbackFloat32 fms(const FallbackFloat32  a, const FallbackFloat32 b, const FallbackFloat32 c) {
	//return a * b - c; 
	return std::fma(a.v, b.v, -c.v);
}

// Fused Negative Multiply Add (-a*b+c)
[[nodiscard("Value calculated and not used (fnma)")]]
inline FallbackFloat32 fnma(const FallbackFloat32  a, const FallbackFloat32 b, const FallbackFloat32 c) { 
	//return -a * b + c; 
	return std::fma(-a.v, b.v, c.v);
}

// Fused Negative Multiply Subtract (-a*b-c)
[[nodiscard("Value calculated and not used (fnms)")]]
inline FallbackFloat32 fnms(const FallbackFloat32  a, const FallbackFloat32 b, const FallbackFloat32 c) { 
	//return -a * b - c; 
	return std::fma(-a.v, b.v, -c.v);
}




//*****Rounding Functions*****
inline FallbackFloat32 floor(FallbackFloat32 a) { return  FallbackFloat32(std::floor(a.v)); }
inline FallbackFloat32 ceil(FallbackFloat32 a) { return  FallbackFloat32(std::ceil(a.v)); }
inline FallbackFloat32 trunc(FallbackFloat32 a) { return  FallbackFloat32(std::trunc(a.v)); }
inline FallbackFloat32 round(FallbackFloat32 a) { return  FallbackFloat32(std::round(a.v)); }
inline FallbackFloat32 fract(FallbackFloat32 a) { return a - floor(a); }


//*****Min/Max*****
inline FallbackFloat32 min(FallbackFloat32 a, FallbackFloat32 b) { return FallbackFloat32(std::min(a.v, b.v)); }
inline FallbackFloat32 max(FallbackFloat32 a, FallbackFloat32 b) { return FallbackFloat32(std::max(a.v, b.v)); }

//Clamp a value between 0.0 and 1.0
[[nodiscard("Value calculated and not used (clamp)")]]
inline FallbackFloat32 clamp(const FallbackFloat32 a) noexcept {
	return std::min(std::max(a.v, 0.0f), 1.0f);
}

//Clamp a value between min and max
[[nodiscard("Value calculated and not used (clamp)")]]
inline FallbackFloat32 clamp(const FallbackFloat32 a, const FallbackFloat32 min_f, const FallbackFloat32 max_f) noexcept {
	return std::min(std::max(a.v, min_f.v), max_f.v);
}

//Clamp a value between min and max
[[nodiscard("Value calculated and not used (clamp)")]]
inline FallbackFloat32 clamp(const FallbackFloat32 a, const float min_f, const float max_f) noexcept {
	return std::min(std::max(a.v, min_f), max_f);
}



//*****Approximate Functions*****
inline FallbackFloat32 reciprocal_approx(FallbackFloat32 a) noexcept { return FallbackFloat32(1.0f / a.v); }

//*****Mathematical Functions*****
inline FallbackFloat32 sqrt(FallbackFloat32 a) { return FallbackFloat32(std::sqrt(a.v)); }
inline FallbackFloat32 abs(FallbackFloat32 a) { return FallbackFloat32(std::abs(a.v)); }
inline FallbackFloat32 pow(FallbackFloat32 a, FallbackFloat32 b) { return FallbackFloat32(std::pow(a.v,b.v)); }


inline FallbackFloat32 sin(FallbackFloat32 a) { return FallbackFloat32(std::sin(a.v)); }
inline FallbackFloat32 cos(FallbackFloat32 a) { return FallbackFloat32(std::cos(a.v)); }
inline FallbackFloat32 tan(FallbackFloat32 a) { return FallbackFloat32(std::tan(a.v)); }
inline FallbackFloat32 asin(FallbackFloat32 a) { return FallbackFloat32(std::asin(a.v)); }
inline FallbackFloat32 acos(FallbackFloat32 a) { return FallbackFloat32(std::acos(a.v)); }
inline FallbackFloat32 atan(FallbackFloat32 a) { return FallbackFloat32(std::atan(a.v)); }
inline FallbackFloat32 atan2(FallbackFloat32 y, FallbackFloat32 x) { return FallbackFloat32(std::atan2(y.v, x.v)); }
inline FallbackFloat32 sinh(FallbackFloat32 a) { return FallbackFloat32(std::sinh(a.v)); }
inline FallbackFloat32 cosh(FallbackFloat32 a) { return FallbackFloat32(std::cosh(a.v)); }
inline FallbackFloat32 tanh(FallbackFloat32 a) { return FallbackFloat32(std::tanh(a.v)); }
inline FallbackFloat32 asinh(FallbackFloat32 a) { return FallbackFloat32(std::asinh(a.v)); }
inline FallbackFloat32 acosh(FallbackFloat32 a) { return FallbackFloat32(std::acosh(a.v)); }
inline FallbackFloat32 atanh(FallbackFloat32 a) { return FallbackFloat32(std::atanh(a.v)); }










//***************** x86_64 only code ******************
#if defined(_M_X64) || defined(__x86_64)
#include <immintrin.h>


/****************************************************************************************************************************************************************************************************
 * SIMD 512 type.  Contains 16 x 32bit Floats
 * Requires AVX-512F support.
 * **************************************************************************************************************************************************************************************************/
struct Simd512Float32 {
	__m512 v;

	typedef float F;
	
	typedef Simd512UInt32 U;
	

	Simd512Float32() = default;
	Simd512Float32(__m512 a) : v(a) {};
	Simd512Float32(F a) : v(_mm512_set1_ps(a)) {};

	
	//*****Support Informtion*****
	
	//Performs a runtime CPU check to see if this type is supported.  Checks this type ONLY (integers in same class may not be supported) 
	static bool cpu_supported() {
		CpuInformation cpuid{};
		cpu_supported(cpuid);
	}

	//Performs a runtime CPU check to see if this type is supported.  Checks this type ONLY (integers in same class may not be supported) 
	static bool cpu_supported(CpuInformation cpuid) {
		return cpuid.has_avx512_f();
	}

	//Performs a runtime CPU check to see if this type's microarchitecture level is supported.  (This will ensure that referernced integer types are also supported)
	static bool cpu_level_supported() {
		CpuInformation cpuid{};
		cpu_level_supported(cpuid);
	}

	//Performs a runtime CPU check to see if this type's microarchitecture level is supported.  (This will ensure that referernced integer types are also supported)
	static bool cpu_level_supported(CpuInformation cpuid) {
		return cpuid.has_avx512_f() && cpuid.has_avx512_dq(); 
	}


	
	
	static constexpr int size_of_element() { return sizeof(float); }
	static constexpr int number_of_elements() { return 16; }

	//*****Access Elements*****
	F element(int i)  const { return v.m512_f32[i]; }
	void set_element(int i, F value) { v.m512_f32[i] = value; }

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

	//*****Negate Operators*****
	Simd512Float32 operator-() const noexcept { return Simd512Float32(_mm512_sub_ps(_mm512_setzero_ps(), v)); }

	//*****Make Functions****
	static Simd512Float32 make_sequential(F first) { return Simd512Float32(_mm512_set_ps(first+15.0f, first + 14.0f, first + 13.0f, first + 12.0f, first + 11.0f, first + 10.0f, first + 9.0f, first + 8.0f, first + 7.0f, first + 6.0f, first + 5.0f, first + 4.0f, first + 3.0f, first + 2.0f, first + 1.0f, first)); }
	

	static Simd512Float32 make_from_int32(Simd512UInt32 i) { return Simd512Float32(_mm512_cvtepu32_ps(i.v)); }

	//*****Cast Functions****

	//Converts to an unsigned integer.  No check is performed to see if that type is supported. Use cpu_level_supported() for safety. 
	Simd512UInt32 bitcast_to_uint() const { return Simd512UInt32(_mm512_castps_si512(this->v)); }
	

	//*****Approximate Functions*****
	//Returns an approximate reciprocal
	[[nodiscard("Value calculated and not used (reciprocal_approx)")]]
	inline Simd512Float32 reciprocal_approx() const noexcept { return Simd512Float32(_mm512_rcp14_ps(v)); }

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


//*****Fused Multiply Add Instructions*****
// Fused Multiply Add (a*b+c)
[[nodiscard("Value calculated and not used (fma)")]]
inline Simd512Float32 fma(const Simd512Float32  a, const Simd512Float32 b, const Simd512Float32 c) {
	return _mm512_fmadd_ps(a.v, b.v, c.v);
}

// Fused Multiply Subtract (a*b-c)
[[nodiscard("Value calculated and not used (fms)")]]
inline Simd512Float32 fms(const Simd512Float32  a, const Simd512Float32 b, const Simd512Float32 c) {
	return _mm512_fmsub_ps(a.v, b.v, c.v);
}

// Fused Negative Multiply Add (-a*b+c)
[[nodiscard("Value calculated and not used (fnma)")]]
inline Simd512Float32 fnma(const Simd512Float32  a, const Simd512Float32 b, const Simd512Float32 c) {
	return _mm512_fnmadd_ps(a.v, b.v, c.v);
}

// Fused Negative Multiply Subtract (-a*b-c)
[[nodiscard("Value calculated and not used (fnms)")]]
inline Simd512Float32 fnms(const Simd512Float32  a, const Simd512Float32 b, const Simd512Float32 c) {
	return _mm512_fnmsub_ps(a.v, b.v, c.v);
}



//*****Rounding Functions*****
[[nodiscard("Value calculated and not used (floor)")]]
inline Simd512Float32 floor(Simd512Float32 a) noexcept { return  Simd512Float32(_mm512_floor_ps(a.v)); }
[[nodiscard("Value calculated and not used (ceil)")]]
inline Simd512Float32 ceil(Simd512Float32 a)  noexcept { return  Simd512Float32(_mm512_ceil_ps(a.v)); }
[[nodiscard("Value calculated and not used (trunc)")]]
inline Simd512Float32 trunc(Simd512Float32 a) noexcept { return  Simd512Float32(_mm512_trunc_ps(a.v)); }
[[nodiscard("Value calculated and not used (round)")]]
inline Simd512Float32 round(Simd512Float32 a) noexcept { return  Simd512Float32(_mm512_roundscale_ps(a.v, _MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC)); }
[[nodiscard("Value calculated and not used (fract)")]]
inline Simd512Float32 fract(Simd512Float32 a) noexcept { return a - floor(a); }

//*****Min/Max*****
[[nodiscard("Value calculated and not used (min)")]]
inline Simd512Float32 min(Simd512Float32 a, Simd512Float32 b) noexcept { return Simd512Float32(_mm512_min_ps(a.v, b.v)); }
[[nodiscard("Value calculated and not used (max)")]]
inline Simd512Float32 max(Simd512Float32 a, Simd512Float32 b) noexcept { return Simd512Float32(_mm512_max_ps(a.v, b.v)); }

//Clamp a value between 0.0 and 1.0
[[nodiscard("Value calculated and not used (clamp)")]]
inline Simd512Float32 clamp(const Simd512Float32 a) noexcept {
	const auto zero = _mm512_setzero_ps();
	const auto one = _mm512_set1_ps(1.0);
	return _mm512_min_ps(_mm512_max_ps(a.v, zero), one);
}

//Clamp a value between min and max
[[nodiscard("Value calculated and not used (clamp)")]]
inline Simd512Float32 clamp(const Simd512Float32 a, const Simd512Float32 min, const Simd512Float32 max) noexcept {
	return _mm512_min_ps(_mm512_max_ps(a.v, min.v), max.v);
}

//Clamp a value between min and max
[[nodiscard("Value calculated and not used (clamp)")]]
inline Simd512Float32 clamp(const Simd512Float32 a, const float min_f, const float max_f) noexcept {
	const auto min = _mm512_set1_ps(min_f);
	const auto max = _mm512_set1_ps(max_f);
	return _mm512_min_ps(_mm512_max_ps(a.v, min), max);
}



//*****Approximate Functions*****
[[nodiscard("Value calculated and not used ()")]]
inline Simd512Float32 reciprocal_approx(Simd512Float32 a) noexcept { return Simd512Float32(_mm512_rcp14_ps(a.v)); }

//*****Mathematical Functions*****
[[nodiscard("Value calculated and not used (sqrt)")]]
inline Simd512Float32 sqrt(Simd512Float32 a) noexcept { return Simd512Float32(_mm512_sqrt_ps(a.v)); }

[[nodiscard("Value calculated and not used (pow)")]]
inline Simd512Float32 pow(Simd512Float32 a, Simd512Float32 b) noexcept { return Simd512Float32(_mm512_pow_ps(a.v,b.v)); }

[[nodiscard("Value calculated and not used (abs)")]]
inline Simd512Float32 abs(Simd512Float32 a) noexcept { return Simd512Float32(_mm512_abs_ps(a.v)); }





//*****Trigonometric Functions *****
[[nodiscard("Value calculated and not used (sin)")]]
inline Simd512Float32 sin(Simd512Float32 a) noexcept { return Simd512Float32(_mm512_sin_ps(a.v)); }

[[nodiscard("Value calculated and not used (cos)")]]
inline Simd512Float32 cos(Simd512Float32 a) noexcept { return Simd512Float32(_mm512_cos_ps(a.v)); }

[[nodiscard("Value calculated and not used (tan)")]]
inline Simd512Float32 tan(Simd512Float32 a) noexcept { return Simd512Float32(_mm512_tan_ps(a.v)); }

[[nodiscard("Value calculated and not used (asin)")]]
inline Simd512Float32 asin(Simd512Float32 a) noexcept { return Simd512Float32(_mm512_asin_ps(a.v)); }

[[nodiscard("Value calculated and not used (acos)")]]
inline Simd512Float32 acos(Simd512Float32 a) noexcept { return Simd512Float32(_mm512_acos_ps(a.v)); }

[[nodiscard("Value calculated and not used (atan)")]]
inline Simd512Float32 atan(Simd512Float32 a) noexcept { return Simd512Float32(_mm512_atan_ps(a.v)); }

[[nodiscard("Value calculated and not used (atan2)")]]
inline Simd512Float32 atan2(Simd512Float32 a, Simd512Float32 b) noexcept { return Simd512Float32(_mm512_atan2_ps(a.v, b.v)); }

[[nodiscard("Value calculated and not used (sinh)")]]
inline Simd512Float32 sinh(Simd512Float32 a) noexcept { return Simd512Float32(_mm512_sinh_ps(a.v)); }

[[nodiscard("Value calculated and not used (cosh)")]]
inline Simd512Float32 cosh(Simd512Float32 a) noexcept { return Simd512Float32(_mm512_cosh_ps(a.v)); }

[[nodiscard("Value calculated and not used (tanh)")]]
inline Simd512Float32 tanh(Simd512Float32 a) noexcept { return Simd512Float32(_mm512_tanh_ps(a.v)); }

[[nodiscard("Value calculated and not used (asinh)")]]
inline Simd512Float32 asinh(Simd512Float32 a) noexcept { return Simd512Float32(_mm512_asinh_ps(a.v)); }

[[nodiscard("Value calculated and not used (acosh)")]]
inline Simd512Float32 acosh(Simd512Float32 a) noexcept { return Simd512Float32(_mm512_acosh_ps(a.v)); }

[[nodiscard("Value calculated and not used (atanh)")]]
inline Simd512Float32 atanh(Simd512Float32 a) noexcept { return Simd512Float32(_mm512_atanh_ps(a.v)); }


/***************************************************************************************************************************************************************************************************
 * SIMD 256 type.  Contains 8 x 32bit Floats
 * Requires AVX support.
 * *************************************************************************************************************************************************************************************************/
struct Simd256Float32 {
	__m256 v;
	typedef float F;
	typedef Simd256UInt32 U;
	typedef Simd256UInt64 U64;


	//*****Constructors*****
	Simd256Float32() = default;
	Simd256Float32(__m256 a) : v(a) {};
	Simd256Float32(F a) :  v(_mm256_set1_ps(a)) {};

	//*****Support Informtion*****
	
	//Performs a runtime CPU check to see if this type is supported.  Checks this type ONLY (integers in same the same level may not be supported) 
	static bool cpu_supported() {
		CpuInformation cpuid{};
		cpu_supported(cpuid);
	}
	//Performs a runtime CPU check to see if this type is supported.  Checks this type ONLY (integers in same the same level may not be supported) 
	static bool cpu_supported(CpuInformation cpuid) {
		return cpuid.has_avx() && cpuid.has_fma();
	}

	//Performs a runtime CPU check to see if this type's microarchitecture level is supported.  (This will ensure that referernced integer types are also supported)
	static bool cpu_level_supported() {
		CpuInformation cpuid{};
		cpu_level_supported(cpuid);
	}

	//Performs a runtime CPU check to see if this type's microarchitecture level is supported.  (This will ensure that referernced integer types are also supported)
	static bool cpu_level_supported(CpuInformation cpuid) {
		return cpuid.has_avx2() && cpuid.has_avx() && cpuid.has_fma();
	}


	static constexpr int size_of_element() { return sizeof(float); }
	static constexpr int number_of_elements() { return 8; }

	//*****Access Elements*****
	F element(int i)  const {return v.m256_f32[i];}
	void set_element(int i, F value) {v.m256_f32[i] = value; }

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

	//*****Negate Operators*****
	Simd256Float32 operator-() const noexcept { return Simd256Float32(_mm256_sub_ps(_mm256_setzero_ps(), v)); }


	//*****Make Functions****
	static Simd256Float32 make_sequential(F first) { return Simd256Float32(_mm256_set_ps(first+7.0f, first + 6.0f, first + 5.0f, first + 4.0f, first + 3.0f, first + 2.0f, first + 1.0f, first)); }
	

	static Simd256Float32 make_from_int32(Simd256UInt32 i) {return Simd256Float32(_mm256_cvtepi32_ps(i.v));}

	//*****Cast Functions****
	
	//Warning: Requires additional CPU features (AVX2)
	Simd256UInt32 bitcast_to_uint() const { return Simd256UInt32(_mm256_castps_si256(this->v)); } 
	

	//*****Approximate Functions*****
	//Returns an approximate reciprocal
	[[nodiscard("Value calculated and not used (reciprocal_approx)")]]
	inline Simd256Float32 reciprocal_approx() const noexcept { return Simd256Float32(_mm256_rcp_ps(v)); }


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

//*****Fused Multiply Add Instructions*****
// Fused Multiply Add (a*b+c)
[[nodiscard("Value calculated and not used (fma)")]]
inline Simd256Float32 fma(const Simd256Float32  a, const Simd256Float32 b, const Simd256Float32 c) {
	return _mm256_fmadd_ps(a.v, b.v, c.v);
}

// Fused Multiply Subtract (a*b-c)
[[nodiscard("Value calculated and not used (fms)")]]
inline Simd256Float32 fms(const Simd256Float32  a, const Simd256Float32 b, const Simd256Float32 c) {
	return _mm256_fmsub_ps(a.v, b.v, c.v);
}

// Fused Negative Multiply Add (-a*b+c)
[[nodiscard("Value calculated and not used (fnma)")]]
inline Simd256Float32 fnma(const Simd256Float32  a, const Simd256Float32 b, const Simd256Float32 c) {
	return _mm256_fnmadd_ps(a.v, b.v, c.v);
}

// Fused Negative Multiply Subtract (-a*b-c)
[[nodiscard("Value calculated and not used (fnms)")]]
inline Simd256Float32 fnms(const Simd256Float32  a, const Simd256Float32 b, const Simd256Float32 c) {
	return _mm256_fnmsub_ps(a.v, b.v, c.v);
}



//*****Rounding Functions*****
[[nodiscard("Value calculated and not used (floor)")]]
inline Simd256Float32 floor(Simd256Float32 a) noexcept {return Simd256Float32(_mm256_floor_ps(a.v));}

[[nodiscard("Value calculated and not used (ceil)")]]
inline Simd256Float32 ceil(Simd256Float32 a) noexcept { return Simd256Float32(_mm256_ceil_ps(a.v));}

[[nodiscard("Value calculated and not used (trunc)")]]
inline Simd256Float32 trunc(Simd256Float32 a) noexcept {return Simd256Float32(_mm256_trunc_ps(a.v));}

[[nodiscard("Value calculated and not used (round)")]]
inline Simd256Float32 round(Simd256Float32 a) noexcept {return Simd256Float32(_mm256_round_ps(a.v, _MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC)); }

[[nodiscard("Value calculated and not used (fract)")]]
inline Simd256Float32 fract(Simd256Float32 a) noexcept {return a - floor(a);}

//*****Min/Max*****
[[nodiscard("Value calculated and not used (min)")]]
inline Simd256Float32 min(const Simd256Float32 a, const Simd256Float32 b)  noexcept {return Simd256Float32(_mm256_min_ps(a.v, b.v)); }

[[nodiscard("Value calculated and not used (max)")]]
inline Simd256Float32 max(const Simd256Float32 a, const Simd256Float32 b)  noexcept {return Simd256Float32(_mm256_max_ps(a.v, b.v));}

//Clamp a value between 0.0 and 1.0
[[nodiscard("Value calculated and not used (clamp)")]]
inline Simd256Float32 clamp(const Simd256Float32 a) noexcept {
	const auto zero = _mm256_setzero_ps();
	const auto one = _mm256_set1_ps(1.0);
	return _mm256_min_ps(_mm256_max_ps(a.v, zero), one);
}

//Clamp a value between min and max
[[nodiscard("Value calculated and not used (clamp)")]]
inline Simd256Float32 clamp(const Simd256Float32 a, const Simd256Float32 min, const Simd256Float32 max) noexcept {
	return _mm256_min_ps(_mm256_max_ps(a.v, min.v), max.v);
}

//Clamp a value between min and max
[[nodiscard("Value calculated and not used (clamp)")]]
inline Simd256Float32 clamp(const Simd256Float32 a, const float min_f, const float max_f) noexcept {
	const auto min = _mm256_set1_ps(min_f);
	const auto max = _mm256_set1_ps(max_f);
	return _mm256_min_ps(_mm256_max_ps(a.v, min), max);
}



//*****Approximate Functions*****
[[nodiscard("Value calculated and not used (reciprocal_approx)")]]
inline Simd256Float32 reciprocal_approx(const Simd256Float32 a) noexcept {return Simd256Float32(_mm256_rcp_ps(a.v));}

//*****Mathematical Functions*****
[[nodiscard("Value calculated and not used (sqrt)")]] 
inline Simd256Float32 sqrt(const Simd256Float32 a) noexcept {return Simd256Float32(_mm256_sqrt_ps(a.v));}

[[nodiscard("Value calculated and not used (pow)")]]
inline Simd256Float32 pow(Simd256Float32 a, Simd256Float32 b) noexcept { return Simd256Float32(_mm256_pow_ps(a.v, b.v)); }

[[nodiscard("Value Calculated and not used (abs)")]]
inline Simd256Float32 abs(const Simd256Float32 a) noexcept {	
	//No AVX for abs so we just flip the bit.
	const auto r = _mm256_and_ps(_mm256_set1_ps(std::bit_cast<float>(0x7FFFFFFF)), a.v); 
	return Simd256Float32(r);
}


//Calculate e^x
[[nodiscard("Value calculated and not used (exp)")]]
inline Simd256Float32 exp(const Simd256Float32 a) noexcept { return Simd256Float32(_mm256_exp_ps(a.v)); }

//Calculate 2^x
[[nodiscard("Value calculated and not used (exp2)")]]
inline Simd256Float32 exp2(const Simd256Float32 a) noexcept { return Simd256Float32(_mm256_exp2_ps(a.v)); }

//Calculate 10^x
[[nodiscard("Value calculated and not used (exp10)")]]
inline Simd256Float32 exp10(const Simd256Float32 a) noexcept { return Simd256Float32(_mm256_exp10_ps(a.v)); }

//Calculate (e^x)-1.0
[[nodiscard("Value calculated and not used (exp_minus1)")]]
inline Simd256Float32 expm1(const Simd256Float32 a) noexcept { return Simd256Float32(_mm256_expm1_ps(a.v)); }

//Calulate natural log(x)
[[nodiscard("Value calculated and not used (log)")]]
inline Simd256Float32 log(const Simd256Float32 a) noexcept { return Simd256Float32(_mm256_log_ps(a.v)); }

//Calulate log(1.0 + x)
[[nodiscard("Value calculated and not used (log1p)")]]
inline Simd256Float32 log1p(const Simd256Float32 a) noexcept { return Simd256Float32(_mm256_log1p_ps(a.v)); }

//Calculate log_1(x)
[[nodiscard("Value calculated and not used (log2)")]]
inline Simd256Float32 log2(const Simd256Float32 a) noexcept { return Simd256Float32(_mm256_log2_ps(a.v)); }

//Calculate log_10(x)
[[nodiscard("Value calculated and not used (log10)")]]
inline Simd256Float32 log10(const Simd256Float32 a) noexcept { return Simd256Float32(_mm256_log10_ps(a.v)); }

//Calculate cube root
[[nodiscard("Value calculated and not used (cbrt)")]]
inline Simd256Float32 cbrt(const Simd256Float32 a) noexcept { return Simd256Float32(_mm256_cbrt_ps(a.v)); }

//Calculate hypot(x).  That is: sqrt(a^2 + b^2) while avoiding overflow.
[[nodiscard("Value calculated and not used (hypot)")]]
inline Simd256Float32 hypot(const Simd256Float32 a, const Simd256Float32 b) noexcept { return Simd256Float32(_mm256_hypot_ps(a.v, b.v)); }




//*****Trigonometric Functions *****

[[nodiscard("Value Calculated and not used (sin)")]]
inline Simd256Float32 sin(const Simd256Float32 a) noexcept {return Simd256Float32(_mm256_sin_ps(a.v));}

[[nodiscard("Value Calculated and not used (cos)")]]
inline Simd256Float32 cos(const Simd256Float32 a)  noexcept {return Simd256Float32(_mm256_cos_ps(a.v));}

[[nodiscard("Value Calculated and not used (tan)")]]
inline Simd256Float32 tan(const Simd256Float32 a) noexcept {return Simd256Float32(_mm256_tan_ps(a.v));}

[[nodiscard("Value Calculated and not used (asin)")]]
inline Simd256Float32 asin(const Simd256Float32 a) noexcept {return Simd256Float32(_mm256_asin_ps(a.v));}

[[nodiscard("Value Calculated and not used (acos)")]]
inline Simd256Float32 acos(const Simd256Float32 a) noexcept {return Simd256Float32(_mm256_acos_ps(a.v));}

[[nodiscard("Value Calculated and not used (atan)")]]
inline Simd256Float32 atan(const Simd256Float32 a) noexcept {return Simd256Float32(_mm256_atan_ps(a.v));}

[[nodiscard("Value Calculated and not used (atan2)")]]
inline Simd256Float32 atan2(const Simd256Float32 a, const Simd256Float32 b) noexcept {return Simd256Float32(_mm256_atan2_ps(a.v, b.v));}

[[nodiscard("Value Calculated and not used (sinh)")]]
inline Simd256Float32 sinh(const Simd256Float32 a) noexcept {return Simd256Float32(_mm256_sinh_ps(a.v));}

[[nodiscard("Value Calculated and not used (cosh)")]]
inline Simd256Float32 cosh(const Simd256Float32 a) noexcept {return Simd256Float32(_mm256_cosh_ps(a.v)); }

[[nodiscard("Value Calculated and not used (tanh)")]]
inline Simd256Float32 tanh(const Simd256Float32 a) noexcept {return Simd256Float32(_mm256_tanh_ps(a.v));}

[[nodiscard("Value Calculated and not used (asinh)")]]
inline Simd256Float32 asinh(const Simd256Float32 a) noexcept {return Simd256Float32(_mm256_asinh_ps(a.v));}

[[nodiscard("Value Calculated and not used (acosh)")]]
inline Simd256Float32 acosh(const Simd256Float32 a) noexcept {return Simd256Float32(_mm256_acosh_ps(a.v));}

[[nodiscard("Value Calculated and not used (atanh)")]]
inline Simd256Float32 atanh(const Simd256Float32 a) noexcept {return Simd256Float32(_mm256_atanh_ps(a.v));}







/***************************************************************************************************************************************************************************************************
 * SIMD 128 type.  Contains 4 x 32bit Floats
 * Requires SSE2 support.  
 * (Improved performance if compiled with SSE4.1)
 * *************************************************************************************************************************************************************************************************/
struct Simd128Float32 {
	__m128 v;
	typedef float F;
	typedef Simd128UInt32 U;
	typedef Simd128UInt64 U64;


	//*****Constructors*****
	Simd128Float32() = default;
	Simd128Float32(__m128 a) : v(a) {};
	Simd128Float32(F a) : v(_mm_set1_ps(a)) {};

	//*****Support Informtion*****

	//Performs a runtime CPU check to see if this type is supported.  Checks this type ONLY (integers in same the same level may not be supported) 
	static bool cpu_supported() {
		CpuInformation cpuid{};
		cpu_supported(cpuid);
	}
	//Performs a runtime CPU check to see if this type is supported.  Checks this type ONLY (integers in same the same level may not be supported) 
	static bool cpu_supported(CpuInformation cpuid) {
		return cpuid.has_sse() && cpuid.has_sse2() && cpuid.has_sse41();
	}

	//Performs a runtime CPU check to see if this type's microarchitecture level is supported.  (This will ensure that referernced integer types are also supported)
	static bool cpu_level_supported() {
		CpuInformation cpuid{};
		cpu_level_supported(cpuid);
	}

	//Performs a runtime CPU check to see if this type's microarchitecture level is supported.  (This will ensure that referernced integer types are also supported)
	static bool cpu_level_supported(CpuInformation cpuid) {
		return cpuid.has_sse() && cpuid.has_sse2() && cpuid.has_sse41();
	}


	static constexpr int size_of_element() { return sizeof(float); }
	static constexpr int number_of_elements() { return 4; }

	//*****Access Elements*****
	F element(int i)  const { return v.m128_f32[i]; }
	void set_element(int i, F value) { v.m128_f32[i] = value; }

	//*****Addition Operators*****
	Simd128Float32& operator+=(const Simd128Float32& rhs) noexcept { v = _mm_add_ps(v, rhs.v); return *this; } //SSE1
	Simd128Float32& operator+=(float rhs) noexcept { v = _mm_add_ps(v, _mm_set1_ps(rhs));	return *this; }

	//*****Subtraction Operators*****
	Simd128Float32& operator-=(const Simd128Float32& rhs) noexcept { v = _mm_sub_ps(v, rhs.v); return *this; }//SSE1
	Simd128Float32& operator-=(float rhs) noexcept { v = _mm_sub_ps(v, _mm_set1_ps(rhs));	return *this; }

	//*****Multiplication Operators*****
	Simd128Float32& operator*=(const Simd128Float32& rhs) noexcept { v = _mm_mul_ps(v, rhs.v); return *this; } //SSE1
	Simd128Float32& operator*=(float rhs) noexcept { v = _mm_mul_ps(v, _mm_set1_ps(rhs)); return *this; }

	//*****Division Operators*****
	Simd128Float32& operator/=(const Simd128Float32& rhs) noexcept { v = _mm_div_ps(v, rhs.v); return *this; } //SSE1
	Simd128Float32& operator/=(float rhs) noexcept { v = _mm_div_ps(v, _mm_set1_ps(rhs));	return *this; }

	//*****Negate Operators*****
	Simd128Float32 operator-() const noexcept { return Simd128Float32(_mm_sub_ps(_mm_setzero_ps(), v)); }


	//*****Make Functions****
	static Simd128Float32 make_sequential(F first) { return Simd128Float32(_mm_set_ps(first + 3.0f, first + 2.0f, first + 1.0f, first)); }


	static Simd128Float32 make_from_int32(Simd128UInt32 i) { return Simd128Float32(_mm_cvtepi32_ps(i.v)); } //SSE2

	//*****Cast Functions****
	Simd128UInt32 bitcast_to_uint() const { return Simd128UInt32(_mm_castps_si128(this->v)); } //SSE2
	

	//*****Approximate Functions*****
	//Returns an approximate reciprocal
	[[nodiscard("Value calculated and not used (reciprocal_approx)")]]
	inline Simd128Float32 reciprocal_approx() const noexcept { return Simd128Float32(_mm_rcp_ps(v)); }  //SSE

};

//*****Addition Operators*****
inline Simd128Float32 operator+(Simd128Float32  lhs, const Simd128Float32& rhs) noexcept { lhs += rhs; return lhs; }
inline Simd128Float32 operator+(Simd128Float32  lhs, float rhs) noexcept { lhs += rhs; return lhs; }
inline Simd128Float32 operator+(float lhs, Simd128Float32 rhs) noexcept { rhs += lhs; return rhs; }

//*****Subtraction Operators*****
inline Simd128Float32 operator-(Simd128Float32  lhs, const Simd128Float32& rhs) noexcept { lhs -= rhs; return lhs; }
inline Simd128Float32 operator-(Simd128Float32  lhs, float rhs) noexcept { lhs -= rhs; return lhs; }
inline Simd128Float32 operator-(const float lhs, const Simd128Float32& rhs) noexcept { return Simd128Float32(_mm_sub_ps(_mm_set1_ps(lhs), rhs.v)); }

//*****Multiplication Operators*****
inline Simd128Float32 operator*(Simd128Float32  lhs, const Simd128Float32& rhs) noexcept { lhs *= rhs; return lhs; }
inline Simd128Float32 operator*(Simd128Float32  lhs, float rhs) noexcept { lhs *= rhs; return lhs; }
inline Simd128Float32 operator*(float lhs, Simd128Float32 rhs) noexcept { rhs *= lhs; return rhs; }

//*****Division Operators*****
inline Simd128Float32 operator/(Simd128Float32  lhs, const Simd128Float32& rhs) noexcept { lhs /= rhs;	return lhs; }
inline Simd128Float32 operator/(Simd128Float32  lhs, float rhs) noexcept { lhs /= rhs; return lhs; }
inline Simd128Float32 operator/(const float lhs, const Simd128Float32& rhs) noexcept { return Simd128Float32(_mm_div_ps(_mm_set1_ps(lhs), rhs.v)); }

//*****Rounding Functions*****
[[nodiscard("Value calculated and not used (floor)")]]
inline Simd128Float32 floor(Simd128Float32 a) noexcept {
	if constexpr (mt::environment::compiler_has_sse4_1) {
		return Simd128Float32(_mm_floor_ps(a.v)); //SSE4.1
	}
	else {
		return Simd128Float32(_mm_set_ps(std::floor(a.v.m128_f32[3]), std::floor(a.v.m128_f32[2]), std::floor(a.v.m128_f32[1]), std::floor(a.v.m128_f32[0])));
	}
} 

[[nodiscard("Value calculated and not used (ceil)")]]
inline Simd128Float32 ceil(Simd128Float32 a) noexcept {
	if constexpr (mt::environment::compiler_has_sse4_1) {
		return Simd128Float32(_mm_ceil_ps(a.v)); //SSE4.1
	}
	else {
		return Simd128Float32(_mm_set_ps(std::ceil(a.v.m128_f32[3]), std::ceil(a.v.m128_f32[2]), std::ceil(a.v.m128_f32[1]), std::ceil(a.v.m128_f32[0])));
	}
}

[[nodiscard("Value calculated and not used (trunc)")]]
inline Simd128Float32 trunc(Simd128Float32 a) noexcept { return Simd128Float32(_mm_trunc_ps(a.v)); } //SSE1

[[nodiscard("Value calculated and not used (round)")]]
inline Simd128Float32 round(Simd128Float32 a) noexcept {
	if constexpr (mt::environment::compiler_has_sse4_1) {
		return Simd128Float32(_mm_round_ps(a.v, _MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC)); //SSE4.1
	}
	else {
		return Simd128Float32(_mm_set_ps(std::round(a.v.m128_f32[3]), std::round(a.v.m128_f32[2]), std::round(a.v.m128_f32[1]), std::round(a.v.m128_f32[0])));
	}
}


[[nodiscard("Value calculated and not used (fract)")]]
inline Simd128Float32 fract(Simd128Float32 a) noexcept { return a - floor(a); } 



//*****Fused Multiply Add Simd128s*****
// Fused Multiply Add (a*b+c)
[[nodiscard("Value calculated and not used (fma)")]]
inline Simd128Float32 fma(const Simd128Float32  a, const Simd128Float32 b, const Simd128Float32 c) {
	if constexpr (mt::environment::compiler_has_avx2) {
		return _mm_fmadd_ps(a.v, b.v, c.v);  //We are compiling to level 3, but using 128 simd.
	}
	else {
		return a * b + c;  //Fallback (no SSE instruction)
	}
} 

// Fused Multiply Subtract (a*b-c)
[[nodiscard("Value calculated and not used (fms)")]]
inline Simd128Float32 fms(const Simd128Float32  a, const Simd128Float32 b, const Simd128Float32 c) {
	if constexpr (mt::environment::compiler_has_avx2) {
		return _mm_fmsub_ps(a.v, b.v, c.v);  //We are compiling to level 3, but using 128 simd.
	}
	else {
		return a * b - c;  //Fallback (no SSE instruction)
	}
} 

// Fused Negative Multiply Add (-a*b+c)
[[nodiscard("Value calculated and not used (fnma)")]]
inline Simd128Float32 fnma(const Simd128Float32  a, const Simd128Float32 b, const Simd128Float32 c) {
	if constexpr (mt::environment::compiler_has_avx2) {
		return _mm_fnmadd_ps(a.v, b.v, c.v);  //We are compiling to level 3, but using 128 simd.
	}
	else {
		return -(a * b) + c;  //Fallback (no SSE instruction)
	}
}

// Fused Negative Multiply Subtract (-a*b-c)
[[nodiscard("Value calculated and not used (fnms)")]]
inline Simd128Float32 fnms(const Simd128Float32  a, const Simd128Float32 b, const Simd128Float32 c) {
	if constexpr (mt::environment::compiler_has_avx2) {
		return _mm_fnmsub_ps(a.v, b.v, c.v); //We are compiling to level 3, but using 128 simd.
	}
	else {
		return -(a * b) - c;  //Fallback (no SSE instruction)
	}
}



//**********Min/Max*v*********
[[nodiscard("Value calculated and not used (min)")]]
inline Simd128Float32 min(const Simd128Float32 a, const Simd128Float32 b)  noexcept { return Simd128Float32(_mm_min_ps(a.v, b.v)); } //SSE1

[[nodiscard("Value calculated and not used (max)")]]
inline Simd128Float32 max(const Simd128Float32 a, const Simd128Float32 b)  noexcept { return Simd128Float32(_mm_max_ps(a.v, b.v)); } //SSE1

//Clamp a value between 0.0 and 1.0
[[nodiscard("Value calculated and not used (clamp)")]]
inline Simd128Float32 clamp(const Simd128Float32 a) noexcept {
	const auto zero = _mm_setzero_ps();
	const auto one = _mm_set1_ps(1.0);
	return _mm_min_ps(_mm_max_ps(a.v, zero), one);
}

//Clamp a value between min and max
[[nodiscard("Value calculated and not used (clamp)")]]
inline Simd128Float32 clamp(const Simd128Float32 a, const Simd128Float32 min, const Simd128Float32 max) noexcept {
	return _mm_min_ps(_mm_max_ps(a.v, min.v), max.v);
}

//Clamp a value between min and max
[[nodiscard("Value calculated and not used (clamp)")]]
inline Simd128Float32 clamp(const Simd128Float32 a, const float min_f, const float max_f) noexcept {
	const auto min = _mm_set1_ps(min_f);
	const auto max = _mm_set1_ps(max_f);
	return _mm_min_ps(_mm_max_ps(a.v, min), max);
}



//*****Approximate Functions*****
[[nodiscard("Value calculated and not used (reciprocal_approx)")]]
inline Simd128Float32 reciprocal_approx(const Simd128Float32 a) noexcept { return Simd128Float32(_mm_rcp_ps(a.v)); } //sse

//*****Mathematical Functions*****
[[nodiscard("Value calculated and not used (sqrt)")]]
inline Simd128Float32 sqrt(const Simd128Float32 a) noexcept { return Simd128Float32(_mm_sqrt_ps(a.v)); } //sse

[[nodiscard("Value calculated and not used (pow)")]]
inline Simd128Float32 pow(Simd128Float32 a, Simd128Float32 b) noexcept { return Simd128Float32(_mm_pow_ps(a.v, b.v)); }

[[nodiscard("Value Calculated and not used (abs)")]]
inline Simd128Float32 abs(const Simd128Float32 a) noexcept {
	//No SSE for abs so we just flip the bit.
	const auto r = _mm_and_ps(_mm_set1_ps(std::bit_cast<float>(0x7FFFFFFF)), a.v);
	return Simd128Float32(r);
}


//*****Trigonometric Functions *****
[[nodiscard("Value Calculated and not used (sin)")]]
inline Simd128Float32 sin(const Simd128Float32 a) noexcept { return Simd128Float32(_mm_sin_ps(a.v)); }  //SSE

[[nodiscard("Value Calculated and not used (cos)")]]
inline Simd128Float32 cos(const Simd128Float32 a)  noexcept { return Simd128Float32(_mm_cos_ps(a.v)); }

[[nodiscard("Value Calculated and not used (tan)")]]
inline Simd128Float32 tan(const Simd128Float32 a) noexcept { return Simd128Float32(_mm_tan_ps(a.v)); }

[[nodiscard("Value Calculated and not used (asin)")]]
inline Simd128Float32 asin(const Simd128Float32 a) noexcept { return Simd128Float32(_mm_asin_ps(a.v)); }

[[nodiscard("Value Calculated and not used (acos)")]]
inline Simd128Float32 acos(const Simd128Float32 a) noexcept { return Simd128Float32(_mm_acos_ps(a.v)); }

[[nodiscard("Value Calculated and not used (atan)")]]
inline Simd128Float32 atan(const Simd128Float32 a) noexcept { return Simd128Float32(_mm_atan_ps(a.v)); }

[[nodiscard("Value Calculated and not used (atan2)")]]
inline Simd128Float32 atan2(const Simd128Float32 a, const Simd128Float32 b) noexcept { return Simd128Float32(_mm_atan2_ps(a.v, b.v)); }

[[nodiscard("Value Calculated and not used (sinh)")]]
inline Simd128Float32 sinh(const Simd128Float32 a) noexcept { return Simd128Float32(_mm_sinh_ps(a.v)); }

[[nodiscard("Value Calculated and not used (cosh)")]]
inline Simd128Float32 cosh(const Simd128Float32 a) noexcept { return Simd128Float32(_mm_cosh_ps(a.v)); }

[[nodiscard("Value Calculated and not used (tanh)")]]
inline Simd128Float32 tanh(const Simd128Float32 a) noexcept { return Simd128Float32(_mm_tanh_ps(a.v)); }

[[nodiscard("Value Calculated and not used (asinh)")]]
inline Simd128Float32 asinh(const Simd128Float32 a) noexcept { return Simd128Float32(_mm_asinh_ps(a.v)); }

[[nodiscard("Value Calculated and not used (acosh)")]]
inline Simd128Float32 acosh(const Simd128Float32 a) noexcept { return Simd128Float32(_mm_acosh_ps(a.v)); }

[[nodiscard("Value Calculated and not used (atanh)")]]
inline Simd128Float32 atanh(const Simd128Float32 a) noexcept { return Simd128Float32(_mm_atanh_ps(a.v)); } //SSE

#endif


/**************************************************************************************************
 * Check that each type implements the desired types from simd-concepts.h
 * (Not sure why this fails on intelisense, but compliles ok.)
 * ************************************************************************************************/
static_assert(Simd<FallbackFloat32>,"FallbackFloat32 does not implement the concept SIMD");
static_assert(SimdReal<FallbackFloat32>, "FallbackFloat32 does not implement the concept SimdReal");
static_assert(SimdFloat<FallbackFloat32>, "FallbackFloat32 does not implement the concept SimdFloat");
static_assert(SimdFloat32<FallbackFloat32>, "FallbackFloat32 does not implement the concept SimdFloat32");
static_assert(SimdFloatToInt<FallbackFloat32>, "FallbackFloat32 does not implement the concept SimdFloatToInt");

#if defined(_M_X64) || defined(__x86_64)
static_assert(Simd<Simd128Float32>, "Simd256Float32 does not implement the concept SIMD");
static_assert(Simd<Simd256Float32>, "Simd256Float32 does not implement the concept SIMD");
static_assert(Simd<Simd512Float32>, "Simd512Float32 does not implement the concept SIMD");

static_assert(SimdReal<Simd128Float32>, "Simd256Float32 does not implement the concept SimdReal");
static_assert(SimdReal<Simd256Float32>, "Simd256Float32 does not implement the concept SimdReal");
static_assert(SimdReal<Simd512Float32>, "Simd512Float32 does not implement the concept SimdReal");

static_assert(SimdFloat<Simd128Float32>, "Simd256Float32 does not implement the concept SimdFloat");
static_assert(SimdFloat<Simd256Float32>, "Simd256Float32 does not implement the concept SimdFloat");
static_assert(SimdFloat<Simd512Float32>, "Simd512Float32 does not implement the concept SimdFloat");

static_assert(SimdFloat32<Simd128Float32>, "Simd128Float32 does not implement the concept SimdFloat32");
static_assert(SimdFloat32<Simd256Float32>, "Simd256Float32 does not implement the concept SimdFloat32");
static_assert(SimdFloat32<Simd512Float32>, "Simd512Float32 does not implement the concept SimdFloat32");

static_assert(SimdFloatToInt<Simd128Float32>, "Simd128Float32 does not implement the concept SimdFloatToInt");
static_assert(SimdFloatToInt<Simd256Float32>, "Simd256Float32 does not implement the concept SimdFloatToInt");
static_assert(SimdFloatToInt<Simd512Float32>, "Simd512Float32 does not implement the concept SimdFloatToInt");

//SIMD Math Support
static_assert(SimdMath<Simd128Float32>, "Simd128Float32 does not implement the concept SimdMath");
static_assert(SimdMath<Simd256Float32>, "Simd256Float32 does not implement the concept SimdMath");
static_assert(SimdMath<Simd512Float32>, "Simd512Float32 does not implement the concept SimdMath");


#endif


/**************************************************************************************************
 Define SimdNativeFloat32 as the best supported type at compile time.  
 (Based on microarchitecture level so that integers are also supported)
 * ************************************************************************************************/
#if defined(_M_X64) || defined(__x86_64)
	#if defined(__AVX512F__) && defined(__AVX512DQ__) 
		typedef Simd512Float32 SimdNativeFloat32;
	#else
		#if defined(__AVX2__) && defined(__AVX__) 
			typedef Simd256Float32 SimdNativeFloat32;
		#else
			#if defined(__SSE4_1__) && defined(__SSE4_1__) && defined(__SSE3__) && defined(__SSSE3__) 
				typedef Simd128Float32 SimdNativeFloat32;
			#else
				typedef Simd128Float32 SimdNativeFloat32;
			#endif	
		#endif	
	#endif
#else
	typedef FallbackFloat32 typedef FallbackFloat32 SimdNativeFloat32;
#endif