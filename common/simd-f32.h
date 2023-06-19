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

Basic SIMD Types for 32-bit floats:

FallbackFloat32		- Works on all build modes and CPUs.  Forwards most requests to the standard library.

Simd128Float32		- x86_64 Microarchitecture Level 1 - Works on all x86_64 CPUs. 
					- Requires SSE and SSE2 support.  Will use SSE4.1 instructions when __SSE4_1__ or __AVX__ defined.

Simd256Float32		- x86_64 Microarchitecture Level 3.
					- Requires AVX, AVX2 and FMA support.

Simd512Float32		- x86_64 Microarchitecture Level 4.
					- Requires AVX512F, AVX512DQ, ACX512VL, AVX512CD, AVX512BW

SimdNativeFloat32	- A Typedef referring to one of the above types.  Chosen based on compiler support/mode.
                    - Just use this type in your code if you are building for a specific platform.


Checking CPU Support:
Unless you are using a SimdNative typedef, you must check for CPU support before using any of these types.

Types reqpresenting floats, doubles, ints, longs etc are arranged in microarchitecture level groups.
Generally CPUs have more SIMD support for floats than ints.
Ensure the CPU supports the full "level" if you need to use more than one type.


To check support at compile time:
	- Use compiler_level_supported()
	- If you won't use any of the type conversion functions you can use compiler_supported()

To check support at run time:
	- Use cpu_level_supported()
	- If you won't use any of the type conversion functions you can use cpu_supported()

Runtime detection notes:
Visual studio will support compiling all types and switching at runtime. However this often results in slower
code than compiling with full compiler support.  Visual studio will optimise AVX code well when build support is enabled.
If you are able, I recommend distributing code at different support levels. (1,3,4). Let the user choose which to download,
or your installer can make the switch.  It is also possible to dynamically load different .dlls

WASM Support:
I've included FallbackFloat32 for use with Emscripen, but use SimdNativeFloat32 as SIMD support will be added soon.


*********************************************************************************************************/
#pragma once

#include <cmath>

#include "environment.h"
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

	//Performs a compile time CPU check to see if this type is supported.  Checks this type ONLY (integers in same the same level may not be supported) 
	static constexpr bool compiler_supported() {
		return true;
	}

	//Performs a compile time support to see if the microarchitecture level is supported.  (This will ensure that referernced integer types are also supported)
	static constexpr bool compiler_level_supported() {
		return true;
	}
	

	
	//*****Access Elements*****
	static constexpr int size_of_element() { return sizeof(float); }
	static constexpr int number_of_elements() { return 1; }	
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
inline FallbackFloat32 exp(FallbackFloat32 a) { return FallbackFloat32(std::exp(a.v)); }
inline FallbackFloat32 exp2(FallbackFloat32 a) { return FallbackFloat32(std::exp2(a.v)); }
inline FallbackFloat32 exp10(FallbackFloat32 a) { return FallbackFloat32(std::pow(10.0f,a.v)); }
inline FallbackFloat32 expm1(FallbackFloat32 a) { return FallbackFloat32(std::expm1(a.v)); }
inline FallbackFloat32 log(FallbackFloat32 a) { return FallbackFloat32(std::log(a.v)); }
inline FallbackFloat32 log1p(FallbackFloat32 a) { return FallbackFloat32(std::log1p(a.v)); }
inline FallbackFloat32 log2(FallbackFloat32 a) { return FallbackFloat32(std::log2(a.v)); }
inline FallbackFloat32 log10(FallbackFloat32 a) { return FallbackFloat32(std::log10(a.v)); }
inline FallbackFloat32 cbrt(FallbackFloat32 a) { return FallbackFloat32(std::cbrt(a.v)); }
inline FallbackFloat32 hypot(FallbackFloat32 a, FallbackFloat32 b) { return FallbackFloat32(std::hypot(a.v, b.v)); }

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


//*****Conditional Functions *****

//Compare if 2 values are equal and return a mask.
inline bool compare_equal(const FallbackFloat32 a, const FallbackFloat32 b) noexcept { return (a.v == b.v); }
inline bool compare_less(const FallbackFloat32 a, const FallbackFloat32 b) noexcept { return(a.v < b.v); }
inline bool compare_less_equal(const FallbackFloat32 a, const FallbackFloat32 b) noexcept { return (a.v <= b.v); }
inline bool compare_greater(const FallbackFloat32 a, const FallbackFloat32 b) noexcept { return (a.v > b.v); }
inline bool compare_greater_equal(const FallbackFloat32 a, const FallbackFloat32 b) noexcept { return (a.v >= b.v); }
inline bool isnan(const FallbackFloat32 a) noexcept { return std::isnan(a.v); }

//Blend two values together based on mask.  First argument if zero. Second argument if 1.
//Note: the if_false argument is first!!
[[nodiscard("Value Calculated and not used (blend)")]]
inline FallbackFloat32 blend(const FallbackFloat32 if_false, const FallbackFloat32 if_true, bool mask) noexcept {
	return (mask) ? if_true : if_false;
}






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

	//Performs a compile time support. Checks this type ONLY (integers in same class may not be supported) 
	static constexpr bool compiler_supported() {
		return mt::environment::compiler_has_avx512f;
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

	//Performs a compile time support to see if the microarchitecture level is supported.  (This will ensure that referernced integer types are also supported)
	static constexpr bool compiler_level_supported() {
		return mt::environment::compiler_has_avx512f && mt::environment::compiler_has_avx512dq && mt::environment::compiler_has_avx512vl && mt::environment::compiler_has_avx512bw && mt::environment::compiler_has_avx512cd;
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



//*****512-bit Mathematical Functions*****
[[nodiscard("Value calculated and not used (sqrt)")]]
inline Simd512Float32 sqrt(Simd512Float32 a) noexcept { return Simd512Float32(_mm512_sqrt_ps(a.v)); }

[[nodiscard("Value calculated and not used (pow)")]]
inline Simd512Float32 pow(Simd512Float32 a, Simd512Float32 b) noexcept { return Simd512Float32(_mm512_pow_ps(a.v,b.v)); }

[[nodiscard("Value calculated and not used (abs)")]]
inline Simd512Float32 abs(Simd512Float32 a) noexcept { return Simd512Float32(_mm512_abs_ps(a.v)); }

//Calculate e^x
[[nodiscard("Value calculated and not used (exp)")]]
inline Simd512Float32 exp(const Simd512Float32 a) noexcept { return Simd512Float32(_mm512_exp_ps(a.v)); }

//Calculate 2^x
[[nodiscard("Value calculated and not used (exp2)")]]
inline Simd512Float32 exp2(const Simd512Float32 a) noexcept { return Simd512Float32(_mm512_exp2_ps(a.v)); }

//Calculate 10^x
[[nodiscard("Value calculated and not used (exp10)")]]
inline Simd512Float32 exp10(const Simd512Float32 a) noexcept { return Simd512Float32(_mm512_exp10_ps(a.v)); }

//Calculate (e^x)-1.0
[[nodiscard("Value calculated and not used (exp_minus1)")]]
inline Simd512Float32 expm1(const Simd512Float32 a) noexcept { return Simd512Float32(_mm512_expm1_ps(a.v)); }

//Calulate natural log(x)
[[nodiscard("Value calculated and not used (log)")]]
inline Simd512Float32 log(const Simd512Float32 a) noexcept { return Simd512Float32(_mm512_log_ps(a.v)); }

//Calulate log(1.0 + x)
[[nodiscard("Value calculated and not used (log1p)")]]
inline Simd512Float32 log1p(const Simd512Float32 a) noexcept { return Simd512Float32(_mm512_log1p_ps(a.v)); }

//Calculate log_1(x)
[[nodiscard("Value calculated and not used (log2)")]]
inline Simd512Float32 log2(const Simd512Float32 a) noexcept { return Simd512Float32(_mm512_log2_ps(a.v)); }

//Calculate log_10(x)
[[nodiscard("Value calculated and not used (log10)")]]
inline Simd512Float32 log10(const Simd512Float32 a) noexcept { return Simd512Float32(_mm512_log10_ps(a.v)); }

//Calculate cube root
[[nodiscard("Value calculated and not used (cbrt)")]]
inline Simd512Float32 cbrt(const Simd512Float32 a) noexcept { return Simd512Float32(_mm512_cbrt_ps(a.v)); }

//Calculate hypot(x).  That is: sqrt(a^2 + b^2) while avoiding overflow.
[[nodiscard("Value calculated and not used (hypot)")]]
inline Simd512Float32 hypot(const Simd512Float32 a, const Simd512Float32 b) noexcept { return Simd512Float32(_mm512_hypot_ps(a.v, b.v)); }





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

//*****AVX-512 Conditional Functions *****

//Compare ordered.
[[nodiscard("Value Calculated and not used (compare_equal)")]]
inline __mmask16 compare_equal(const Simd512Float32 a, const Simd512Float32 b) noexcept { return _mm512_cmp_ps_mask(a.v, b.v, _CMP_EQ_OQ); }

//Compare ordered.
[[nodiscard("Value Calculated and not used (compare_less)")]]
inline __mmask16 compare_less(const Simd512Float32 a, const Simd512Float32 b) noexcept { return _mm512_cmp_ps_mask(a.v, b.v, _CMP_LT_OQ); }

//Compare ordered.
[[nodiscard("Value Calculated and not used (compare_less_equal)")]]
inline __mmask16 compare_less_equal(const Simd512Float32 a, const Simd512Float32 b) noexcept { return _mm512_cmp_ps_mask(a.v, b.v, _CMP_LE_OQ); }

//Compare ordered.
[[nodiscard("Value Calculated and not used (compare_greater)")]]
inline __mmask16 compare_greater(const Simd512Float32 a, const Simd512Float32 b) noexcept { return _mm512_cmp_ps_mask(a.v, b.v, _CMP_GT_OQ); }

//Compare ordered.
[[nodiscard("Value Calculated and not used (compare_greater_equal)")]]
inline __mmask16 compare_greater_equal(const Simd512Float32 a, const Simd512Float32 b) noexcept { return _mm512_cmp_ps_mask(a.v, b.v, _CMP_GE_OQ); }

//Compare to nan
[[nodiscard("Value Calculated and not used (compare_is_nan)")]]
inline __mmask16 isnan(const Simd512Float32 a) noexcept { return _mm512_cmp_ps_mask(a.v, a.v, _CMP_UNORD_Q); }


//Blend two values together based on mask.First argument if zero.Second argument if 1.
//Note: the if_false argument is first!!
[[nodiscard("Value Calculated and not used (blend)")]]
inline Simd512Float32 blend(const Simd512Float32 if_false, const Simd512Float32 if_true, __mmask16 mask) noexcept {
	return Simd512Float32(_mm512_mask_blend_ps(mask, if_false.v, if_true.v));
}




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

	//Performs a compile time support. Checks this type ONLY (integers in same class may not be supported) 
	static constexpr bool compiler_supported() {
		return mt::environment::compiler_has_avx && mt::environment::compiler_has_fma;
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

	//Performs a compile time support to see if the microarchitecture level is supported.  (This will ensure that referernced integer types are also supported)
	static constexpr bool compiler_level_supported() {
		return mt::environment::compiler_has_avx2 && mt::environment::compiler_has_avx && mt::environment::compiler_has_fma;
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



//*****256-bit SIMD Mathematical Functions*****
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

//*****Conditional Functions *****

//Compare ordered.
inline __m256 compare_equal(const Simd256Float32 a, const Simd256Float32 b) noexcept { return _mm256_cmp_ps(a.v, b.v, _CMP_EQ_OQ); }
inline __m256 compare_less(const Simd256Float32 a, const Simd256Float32 b) noexcept { return _mm256_cmp_ps(a.v, b.v,  _CMP_LT_OS); }
inline __m256 compare_less_equal(const Simd256Float32 a, const Simd256Float32 b) noexcept { return _mm256_cmp_ps(a.v, b.v, _CMP_LE_OS); }
inline __m256 compare_greater(const Simd256Float32 a, const Simd256Float32 b) noexcept { return _mm256_cmp_ps(a.v, b.v, _CMP_GT_OS); }
inline __m256 compare_greater_equal(const Simd256Float32 a, const Simd256Float32 b) noexcept { return _mm256_cmp_ps(a.v, b.v, _CMP_GE_OS); }
inline __m256 isnan(const Simd256Float32 a) noexcept { return _mm256_cmp_ps(a.v, a.v, _CMP_UNORD_Q); }

//Blend two values together based on mask.First argument if zero.Second argument if 1.
//Note: the if_false argument is first!!
[[nodiscard("Value Calculated and not used (blend)")]]
inline Simd256Float32 blend(const Simd256Float32 if_false, const Simd256Float32 if_true, __m256 mask) noexcept {
	return Simd256Float32(_mm256_blendv_ps(if_false.v, if_true.v, mask));	
}





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

	//Performs a compile time support. Checks this type ONLY (integers in same class may not be supported) 
	static constexpr bool compiler_supported() {
		return mt::environment::compiler_has_sse && mt::environment::compiler_has_sse2;
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

	//Performs a compile time support to see if the microarchitecture level is supported.  (This will ensure that referernced integer types are also supported)
	static constexpr bool compiler_level_supported() {
		return mt::environment::compiler_has_sse && mt::environment::compiler_has_sse2;
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




//*****128-bit SIMD Mathematical Functions*****

//Calculate square root.
[[nodiscard("Value calculated and not used (sqrt)")]]
inline Simd128Float32 sqrt(const Simd128Float32 a) noexcept { return Simd128Float32(_mm_sqrt_ps(a.v)); } //sse

//Calculating a raised to the power of b
[[nodiscard("Value calculated and not used (pow)")]]
inline Simd128Float32 pow(Simd128Float32 a, Simd128Float32 b) noexcept { return Simd128Float32(_mm_pow_ps(a.v, b.v)); }

//Calculate the absoulte value.  Performed by unsetting the sign bit.
[[nodiscard("Value Calculated and not used (abs)")]]
inline Simd128Float32 abs(const Simd128Float32 a) noexcept {
	//No SSE for abs so we just flip the bit.
	const auto r = _mm_and_ps(_mm_set1_ps(std::bit_cast<float>(0x7FFFFFFF)), a.v);
	return Simd128Float32(r);
}

//Calculate e^x
[[nodiscard("Value calculated and not used (exp)")]]
inline Simd128Float32 exp(const Simd128Float32 a) noexcept { return Simd128Float32(_mm_exp_ps(a.v)); } //sse

//Calculate 2^x
[[nodiscard("Value calculated and not used (exp2)")]]
inline Simd128Float32 exp2(const Simd128Float32 a) noexcept { return Simd128Float32(_mm_exp2_ps(a.v)); } //sse

//Calculate 10^x
[[nodiscard("Value calculated and not used (exp10)")]]
inline Simd128Float32 exp10(const Simd128Float32 a) noexcept { return Simd128Float32(_mm_exp10_ps(a.v)); } //sse

//Calculate (e^x)-1.0
[[nodiscard("Value calculated and not used (exp_minus1)")]]
inline Simd128Float32 expm1(const Simd128Float32 a) noexcept { return Simd128Float32(_mm_expm1_ps(a.v)); } //sse

//Calulate natural log(x)
[[nodiscard("Value calculated and not used (log)")]]
inline Simd128Float32 log(const Simd128Float32 a) noexcept { return Simd128Float32(_mm_log_ps(a.v)); } //sse

//Calulate log(1.0 + x)
[[nodiscard("Value calculated and not used (log1p)")]]
inline Simd128Float32 log1p(const Simd128Float32 a) noexcept { return Simd128Float32(_mm_log1p_ps(a.v)); } //sse

//Calculate log_1(x)
[[nodiscard("Value calculated and not used (log2)")]]
inline Simd128Float32 log2(const Simd128Float32 a) noexcept { return Simd128Float32(_mm_log2_ps(a.v)); } //sse

//Calculate log_10(x)
[[nodiscard("Value calculated and not used (log10)")]]
inline Simd128Float32 log10(const Simd128Float32 a) noexcept { return Simd128Float32(_mm_log10_ps(a.v)); } //sse

//Calculate cube root
[[nodiscard("Value calculated and not used (cbrt)")]]
inline Simd128Float32 cbrt(const Simd128Float32 a) noexcept { return Simd128Float32(_mm_cbrt_ps(a.v)); } //sse


//Calculate hypot(x).  That is: sqrt(a^2 + b^2) while avoiding overflow.
[[nodiscard("Value calculated and not used (hypot)")]]
inline Simd128Float32 hypot(const Simd128Float32 a, const Simd128Float32 b) noexcept { return Simd128Float32(_mm_hypot_ps(a.v, b.v)); } //sse



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



//*****Conditional Functions *****

//Compare if 2 values are equal and return a mask.
inline __m128 compare_equal(const Simd128Float32 a, const Simd128Float32 b) noexcept { return _mm_cmpeq_ps(a.v, b.v); }
inline __m128 compare_less(const Simd128Float32 a, const Simd128Float32 b) noexcept { return _mm_cmplt_ps(a.v, b.v); }
inline __m128 compare_less_equal(const Simd128Float32 a, const Simd128Float32 b) noexcept { return _mm_cmple_ps(a.v, b.v); }
inline __m128 compare_greater(const Simd128Float32 a, const Simd128Float32 b) noexcept { return _mm_cmpgt_ps(a.v, b.v); }
inline __m128 compare_greater_equal(const Simd128Float32 a, const Simd128Float32 b) noexcept { return _mm_cmpge_ps(a.v, b.v); }
inline __m128 isnan(const Simd128Float32 a) noexcept { return _mm_cmpunord_ps(a.v, a.v); }

//Blend two values together based on mask.  First argument if zero. Second argument if 1.
//Note: the if_false argument is first!!
[[nodiscard("Value Calculated and not used (blend)")]]
inline Simd128Float32 blend(const Simd128Float32 if_false, const Simd128Float32 if_true, __m128 mask) noexcept { 
	if constexpr (mt::environment::compiler_has_sse4_1) {
		return Simd128Float32(_mm_blendv_ps(if_false.v, if_true.v, mask));
	}
	else {
		return Simd128Float32(_mm_or_ps(_mm_andnot_ps(mask,if_false.v), _mm_and_ps(mask, if_true.v)));
	}
}





#endif

/**************************************************************************************************
 * Templated Functions for all types
 * ************************************************************************************************/

//If values a and b are equal return if_true, otherwise return if_false.
template <SimdFloat32 T> 
[[nodiscard("Value Calculated and not used (if_equal)")]]
inline T if_equal(const T value_a, const T value_b, const T if_true, const T if_false) noexcept {
	return blend(if_false, if_true, compare_equal(value_a, value_b));
}

template <SimdFloat32 T>
[[nodiscard("Value Calculated and not used (if_less)")]]
inline T if_less(const T value_a, const T value_b, const T if_true, const T if_false) noexcept {
	return blend(if_false, if_true, compare_less(value_a, value_b));
}

template <SimdFloat32 T>
[[nodiscard("Value Calculated and not used (if_less_equal)")]]
inline T if_less_equal(const T value_a, const T value_b, const T if_true, const T if_false) noexcept {
	return blend(if_false, if_true, compare_less_equal(value_a, value_b));
}

template <SimdFloat32 T>
[[nodiscard("Value Calculated and not used (if_greater)")]]
inline T if_greater(const T value_a, const T value_b, const T if_true, const T if_false) noexcept {
	return blend(if_false, if_true, compare_greater(value_a, value_b));
}


template <SimdFloat32 T>
[[nodiscard("Value Calculated and not used (if_greater_equal)")]]
inline T if_greater_equal(const T value_a, const T value_b, const T if_true, const T if_false) noexcept {
	return blend(if_false, if_true, compare_greater_equal(value_a, value_b));
}


template <SimdFloat32 T>
[[nodiscard("Value Calculated and not used (if_nan)")]]
inline T if_nan(const T value_a, const T if_true, const T if_false) noexcept {
	return blend(if_false, if_true, isnan(value_a));
}





/**************************************************************************************************
 * Check that each type implements the desired types from simd-concepts.h
 * (Not sure why this fails on intelisense, but compliles ok.)
 * ************************************************************************************************/
static_assert(Simd<FallbackFloat32>,"FallbackFloat32 does not implement the concept SIMD");
static_assert(SimdReal<FallbackFloat32>, "FallbackFloat32 does not implement the concept SimdReal");
static_assert(SimdFloat<FallbackFloat32>, "FallbackFloat32 does not implement the concept SimdFloat");
static_assert(SimdFloat32<FallbackFloat32>, "FallbackFloat32 does not implement the concept SimdFloat32");
static_assert(SimdFloatToInt<FallbackFloat32>, "FallbackFloat32 does not implement the concept SimdFloatToInt");
static_assert(SimdMath<FallbackFloat32>, "FallbackFloat32 does not implement the concept SimdMath");
static_assert(SimdCompareOps<FallbackFloat32>, "FallbackFloat32 does not implement the concept SimdCompareOps");


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

//Compare Ops
static_assert(SimdCompareOps<Simd128Float32>, "Simd128Float32 does not implement the concept SimdCompareOps");
static_assert(SimdCompareOps<Simd256Float32>, "Simd256Float32 does not implement the concept SimdCompareOps");
static_assert(SimdCompareOps<Simd512Float32>, "Simd512Float32 does not implement the concept SimdCompareOps");

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
	//non x64
	typedef FallbackFloat32 SimdNativeFloat32;
#endif