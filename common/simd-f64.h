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

Basic SIMD Types for 64-bit floats.

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



#include "simd-cpuid.h"
#include "simd-concepts.h"
#include <cmath>





/**************************************************************************************************
 * Fallback 64-Bit Floating Point
 *
 * ************************************************************************************************/
struct FallbackFloat64 {
	double v;

	typedef double F;				//The type of the underlying data.
	typedef FallbackUInt64 U;		//The type if cast to an unsigned int.
	typedef FallbackUInt64 U64;		//The type of a 64-bit unsigned int.

	FallbackFloat64() = default;
	FallbackFloat64(F a) : v(a) {};




	//*****Support Informtion*****
#if defined(_M_X64) || defined(__x86_64)
	static bool cpu_supported(CpuInformation) {
		return true;
	}
#endif
	static bool cpu_supported() { return true; }

	//*****Access Elements*****
	F element(int) const { return v; }
	void set_element(int, F value) { v = value; }
	static constexpr int size_of_element() { return sizeof(double); }
	static constexpr int number_of_elements() { return 1; }

	//*****Addition Operators*****
	FallbackFloat64& operator+=(const FallbackFloat64& rhs) noexcept { v += rhs.v; return *this; }
	FallbackFloat64& operator+=(double rhs) noexcept { v += rhs;	return *this; }

	//*****Subtraction Operators*****
	FallbackFloat64& operator-=(const FallbackFloat64& rhs) noexcept { v -= rhs.v; return *this; }
	FallbackFloat64& operator-=(double rhs) noexcept { v -= rhs;	return *this; }

	//*****Multiplication Operators*****
	FallbackFloat64& operator*=(const FallbackFloat64& rhs) noexcept { v *= rhs.v; return *this; }
	FallbackFloat64& operator*=(double rhs) noexcept { v *= rhs; return *this; }

	//*****Division Operators*****
	FallbackFloat64& operator/=(const FallbackFloat64& rhs) noexcept { v /= rhs.v; return *this; }
	FallbackFloat64& operator/=(double rhs) noexcept { v /= rhs;	return *this; }

	//*****Negate Operator*****
	FallbackFloat64 operator-() const noexcept { return FallbackFloat64(-v); }

	//*****Make Functions****
	static FallbackFloat64 make_sequential(F first) { return FallbackFloat64(first); }
	static FallbackFloat64 make_set1(F v) { return FallbackFloat64(v); }

	static FallbackFloat64 make_from_uints_52bits(FallbackUInt64 i) {
		auto x = i.v & 0b0000000000001111111111111111111111111111111111111111111111111111; //mask of 52-bits.
		auto f = static_cast<F>(x);
		return FallbackFloat64(f);
	}


	//*****Cast Functions****
	FallbackUInt64 bitcast_to_uint64() const { return FallbackUInt64(std::bit_cast<uint64_t>(this->v)); }

	//*****Approximate Functions*****
	inline FallbackFloat64 reciprocal_approx() const noexcept { return FallbackFloat64(1.0f / v); }



};

//*****Addition Operators*****
inline FallbackFloat64 operator+(FallbackFloat64  lhs, const FallbackFloat64& rhs) noexcept { lhs += rhs; return lhs; }
inline FallbackFloat64 operator+(FallbackFloat64  lhs, double rhs) noexcept { lhs += rhs; return lhs; }
inline FallbackFloat64 operator+(double lhs, FallbackFloat64 rhs) noexcept { rhs += lhs; return rhs; }

//*****Subtraction Operators*****
inline FallbackFloat64 operator-(FallbackFloat64  lhs, const FallbackFloat64& rhs) noexcept { lhs -= rhs; return lhs; }
inline FallbackFloat64 operator-(FallbackFloat64  lhs, double rhs) noexcept { lhs -= rhs; return lhs; }
inline FallbackFloat64 operator-(const double lhs, const FallbackFloat64& rhs) noexcept { return FallbackFloat64(lhs - rhs.v); }

//*****Multiplication Operators*****
inline FallbackFloat64 operator*(FallbackFloat64  lhs, const FallbackFloat64& rhs) noexcept { lhs *= rhs; return lhs; }
inline FallbackFloat64 operator*(FallbackFloat64  lhs, double rhs) noexcept { lhs *= rhs; return lhs; }
inline FallbackFloat64 operator*(double lhs, FallbackFloat64 rhs) noexcept { rhs *= lhs; return rhs; }

//*****Division Operators*****
inline FallbackFloat64 operator/(FallbackFloat64  lhs, const FallbackFloat64& rhs) noexcept { lhs /= rhs;	return lhs; }
inline FallbackFloat64 operator/(FallbackFloat64  lhs, double rhs) noexcept { lhs /= rhs; return lhs; }
inline FallbackFloat64 operator/(const double lhs, const FallbackFloat64& rhs) noexcept { return FallbackFloat64(lhs - rhs.v); }

//*****Fused Multiply Add Fallbacks*****
// Fused Multiply Add (a*b+c)
[[nodiscard("Value calculated and not used (fma)")]]
inline FallbackFloat64 fma(const FallbackFloat64  a, const FallbackFloat64 b, const FallbackFloat64 c) { return a * b + c; }

// Fused Multiply Subtract (a*b-c)
[[nodiscard("Value calculated and not used (fms)")]]
inline FallbackFloat64 fms(const FallbackFloat64  a, const FallbackFloat64 b, const FallbackFloat64 c) { return a * b - c; }

// Fused Negative Multiply Add (-a*b+c)
[[nodiscard("Value calculated and not used (fnma)")]]
inline FallbackFloat64 fnma(const FallbackFloat64  a, const FallbackFloat64 b, const FallbackFloat64 c) { return -a * b + c; }

// Fused Negative Multiply Subtract (-a*b-c)
[[nodiscard("Value calculated and not used (fnms)")]]
inline FallbackFloat64 fnms(const FallbackFloat64  a, const FallbackFloat64 b, const FallbackFloat64 c) { return -a * b - c; }

//*****Rounding Functions*****
inline FallbackFloat64 floor(FallbackFloat64 a) { return  FallbackFloat64(std::floor(a.v)); }
inline FallbackFloat64 ceil(FallbackFloat64 a) { return  FallbackFloat64(std::ceil(a.v)); }
inline FallbackFloat64 trunc(FallbackFloat64 a) { return  FallbackFloat64(std::trunc(a.v)); }
inline FallbackFloat64 round(FallbackFloat64 a) { return  FallbackFloat64(std::round(a.v)); }
inline FallbackFloat64 fract(FallbackFloat64 a) { return a - floor(a); }


//*****Min/Max*****
inline FallbackFloat64 min(FallbackFloat64 a, FallbackFloat64 b) { return FallbackFloat64(std::min(a.v, b.v)); }
inline FallbackFloat64 max(FallbackFloat64 a, FallbackFloat64 b) { return FallbackFloat64(std::max(a.v, b.v)); }

//*****Approximate Functions*****
inline FallbackFloat64 reciprocal_approx(FallbackFloat64 a) { return FallbackFloat64(1.0f / a.v); }

//*****Mathematical Functions*****
inline FallbackFloat64 sqrt(FallbackFloat64 a) { return FallbackFloat64(std::sqrt(a.v)); }
inline FallbackFloat64 sin(FallbackFloat64 a) { return FallbackFloat64(std::sin(a.v)); }
inline FallbackFloat64 cos(FallbackFloat64 a) { return FallbackFloat64(std::cos(a.v)); }
inline FallbackFloat64 tan(FallbackFloat64 a) { return FallbackFloat64(std::tan(a.v)); }
inline FallbackFloat64 asin(FallbackFloat64 a) { return FallbackFloat64(std::asin(a.v)); }
inline FallbackFloat64 acos(FallbackFloat64 a) { return FallbackFloat64(std::acos(a.v)); }
inline FallbackFloat64 atan(FallbackFloat64 a) { return FallbackFloat64(std::atan(a.v)); }
inline FallbackFloat64 atan2(FallbackFloat64 y, FallbackFloat64 x) { return FallbackFloat64(std::atan2(x.v, y.v)); }
inline FallbackFloat64 sinh(FallbackFloat64 a) { return FallbackFloat64(std::sinh(a.v)); }
inline FallbackFloat64 cosh(FallbackFloat64 a) { return FallbackFloat64(std::cosh(a.v)); }
inline FallbackFloat64 tanh(FallbackFloat64 a) { return FallbackFloat64(std::tanh(a.v)); }
inline FallbackFloat64 asinh(FallbackFloat64 a) { return FallbackFloat64(std::asinh(a.v)); }
inline FallbackFloat64 acosh(FallbackFloat64 a) { return FallbackFloat64(std::acosh(a.v)); }
inline FallbackFloat64 atanh(FallbackFloat64 a) { return FallbackFloat64(std::atanh(a.v)); }
inline FallbackFloat64 abs(FallbackFloat64 a) { return FallbackFloat64(std::abs(a.v)); }

//Clamp a value between 0.0 and 1.0
[[nodiscard("Value calculated and not used (clamp)")]]
inline FallbackFloat64 clamp(const FallbackFloat64 a) noexcept {
	return std::min(std::max(a.v, 0.0), 1.0);
}

//Clamp a value between min and max
[[nodiscard("Value calculated and not used (clamp)")]]
inline FallbackFloat64 clamp(const FallbackFloat64 a, const FallbackFloat64 min_f, const FallbackFloat64 max_f) noexcept {
	return std::min(std::max(a.v, min_f.v), max_f.v);
}

//Clamp a value between min and max
[[nodiscard("Value calculated and not used (clamp)")]]
inline FallbackFloat64 clamp(const FallbackFloat64 a, const double min_f, const double max_f) noexcept {
	return std::min(std::max(a.v, min_f), max_f);
}











//***************** x86_64 only code ******************
#if defined(_M_X64) || defined(__x86_64)
#include <immintrin.h>



/****************************************************************************************************************************************************************************************************
 * SIMD 512 type.  Contains 16 x 64bit Floats
 * Requires AVX-512F support.
 * **************************************************************************************************************************************************************************************************/
struct Simd512Float64 {
	__m512d v;

	typedef double F;	
	typedef Simd512UInt64 U;
	

	Simd512Float64() = default;
	Simd512Float64(__m512d a) : v(a) {};
	Simd512Float64(F a) : v(_mm512_set1_pd(a)) {}

	//*****Support Informtion*****
	static bool cpu_supported(CpuInformation cpuid) {
		return cpuid.has_avx512_f();
	}
	static constexpr int size_of_element() { return sizeof(double); }
	static constexpr int number_of_elements() { return 8; }

	//*****Access Elements*****
	F element(int i) const { return v.m512d_f64[i]; }
	void set_element(int i, F value) { v.m512d_f64[i] = value; }

	//*****Addition Operators*****
	Simd512Float64& operator+=(const Simd512Float64& rhs) noexcept { v = _mm512_add_pd(v, rhs.v); return *this; }
	Simd512Float64& operator+=(double rhs) noexcept { v = _mm512_add_pd(v, _mm512_set1_pd(rhs));	return *this; }

	//*****Subtraction Operators*****
	Simd512Float64& operator-=(const Simd512Float64& rhs) noexcept { v = _mm512_sub_pd(v, rhs.v); return *this; }
	Simd512Float64& operator-=(double rhs) noexcept { v = _mm512_sub_pd(v, _mm512_set1_pd(rhs));	return *this; }

	//*****Multiplication Operators*****
	Simd512Float64& operator*=(const Simd512Float64& rhs) noexcept { v = _mm512_mul_pd(v, rhs.v); return *this; }
	Simd512Float64& operator*=(double rhs) noexcept { v = _mm512_mul_pd(v, _mm512_set1_pd(rhs)); return *this; }

	//*****Division Operators*****
	Simd512Float64& operator/=(const Simd512Float64& rhs) noexcept { v = _mm512_div_pd(v, rhs.v); return *this; }
	Simd512Float64& operator/=(double rhs) noexcept { v = _mm512_div_pd(v, _mm512_set1_pd(rhs));	return *this; }

	//*****Negate Operator*****
	Simd512Float64 operator-() const noexcept { return Simd512Float64(_mm512_sub_pd(_mm512_setzero_pd(), v)); }

	//*****Make Functions****
	static Simd512Float64 make_sequential(F first) { return Simd512Float64(_mm512_set_pd(first + 7.0f, first + 6.0f, first + 5.0f, first + 4.0f, first + 3.0f, first + 2.0f, first + 1.0f, first)); }
	static Simd512Float64 make_set1(F v) { return _mm512_set1_pd(v); }

	static Simd512Float64 make_from_uints_52bits(Simd512UInt64 i) {
		auto x = _mm512_and_si512(i.v, _mm512_set1_epi64(0b0000000000001111111111111111111111111111111111111111111111111111)); //mask of 52-bits.
		auto u = _mm512_cvtepu64_pd(x); //avx-512dq instruction
		return Simd512Float64(u);
	}

	//*****Cast Functions****

	//Warning: Returned type requires additional CPU features (AVX-512DQ)
	Simd512UInt64 bitcast_to_uint64() const { return Simd512UInt64(_mm512_castpd_si512(this->v)); }

	//*****Approximate Functions*****
	//Returns an approximate reciprocal
	[[nodiscard("Value calculated and not used (reciprocal_approx)")]]
	inline Simd512Float64 reciprocal_approx() const noexcept { return Simd512Float64(_mm512_rcp14_pd(v)); }



};

//*****Addition Operators*****
inline Simd512Float64 operator+(Simd512Float64 lhs, const Simd512Float64& rhs) noexcept { lhs += rhs; return lhs; }
inline Simd512Float64 operator+(Simd512Float64 lhs, double rhs) noexcept { lhs += rhs; return lhs; }
inline Simd512Float64 operator+(double lhs, Simd512Float64 rhs) noexcept { rhs += lhs; return rhs; }

//*****Subtraction Operators*****
inline Simd512Float64 operator-(Simd512Float64  lhs, const Simd512Float64& rhs) noexcept { lhs -= rhs; return lhs; }
inline Simd512Float64 operator-(Simd512Float64  lhs, double rhs) noexcept { lhs -= rhs; return lhs; }
inline Simd512Float64 operator-(const double lhs, const Simd512Float64& rhs) noexcept { return Simd512Float64(_mm512_sub_pd(_mm512_set1_pd(lhs), rhs.v)); }

//*****Multiplication Operators*****
inline Simd512Float64 operator*(Simd512Float64  lhs, const Simd512Float64& rhs) noexcept { lhs *= rhs; return lhs; }
inline Simd512Float64 operator*(Simd512Float64  lhs, double rhs) noexcept { lhs *= rhs; return lhs; }
inline Simd512Float64 operator*(double lhs, Simd512Float64 rhs) noexcept { rhs *= lhs; return rhs; }

//*****Division Operators*****
inline Simd512Float64 operator/(Simd512Float64  lhs, const Simd512Float64& rhs) noexcept { lhs /= rhs;	return lhs; }
inline Simd512Float64 operator/(Simd512Float64  lhs, double rhs) noexcept { lhs /= rhs; return lhs; }
inline Simd512Float64 operator/(const double lhs, const Simd512Float64& rhs) noexcept { return Simd512Float64(_mm512_div_pd(_mm512_set1_pd(lhs), rhs.v)); }

//*****Fused Multiply Add Instructions*****
// Fused Multiply Add (a*b+c)
[[nodiscard("Value calculated and not used (fma)")]]
inline Simd512Float64 fma(const Simd512Float64  a, const Simd512Float64 b, const Simd512Float64 c) {
	return _mm512_fmadd_pd(a.v, b.v, c.v);
}

// Fused Multiply Subtract (a*b-c)
[[nodiscard("Value calculated and not used (fms)")]]
inline Simd512Float64 fms(const Simd512Float64  a, const Simd512Float64 b, const Simd512Float64 c) {
	return _mm512_fmsub_pd(a.v, b.v, c.v);
}

// Fused Negative Multiply Add (-a*b+c)
[[nodiscard("Value calculated and not used (fnma)")]]
inline Simd512Float64 fnma(const Simd512Float64  a, const Simd512Float64 b, const Simd512Float64 c) {
	return _mm512_fnmadd_pd(a.v, b.v, c.v);
}

// Fused Negative Multiply Subtract (-a*b-c)
[[nodiscard("Value calculated and not used (fnms)")]]
inline Simd512Float64 fnms(const Simd512Float64  a, const Simd512Float64 b, const Simd512Float64 c) {
	return _mm512_fnmsub_pd(a.v, b.v, c.v);
}

//*****Rounding Functions*****
inline Simd512Float64 floor(Simd512Float64 a) {return  Simd512Float64(_mm512_floor_pd(a.v)); }
inline Simd512Float64 ceil(Simd512Float64 a) { return  Simd512Float64(_mm512_ceil_pd(a.v)); }
inline Simd512Float64 trunc(Simd512Float64 a) { return  Simd512Float64(_mm512_trunc_pd(a.v)); }
inline Simd512Float64 round(Simd512Float64 a) { return  Simd512Float64(_mm512_roundscale_pd(a.v, _MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC)); }
inline Simd512Float64 fract(Simd512Float64 a) { return a - floor(a);}

//*****Min/Max*****
inline Simd512Float64 min(Simd512Float64 a, Simd512Float64 b) { return Simd512Float64(_mm512_min_pd(a.v,b.v)); }
inline Simd512Float64 max(Simd512Float64 a, Simd512Float64 b) { return Simd512Float64(_mm512_max_pd(a.v, b.v)); }


//*****Approximate Functions*****
inline Simd512Float64 reciprocal_approx(Simd512Float64 a) { return Simd512Float64(_mm512_rcp14_pd(a.v)); }

//*****Mathematical Functions*****
inline Simd512Float64 sqrt(Simd512Float64 a) { return Simd512Float64(_mm512_sqrt_pd(a.v)); }
inline Simd512Float64 sin(Simd512Float64 a) { return Simd512Float64(_mm512_sin_pd(a.v)); }
inline Simd512Float64 cos(Simd512Float64 a) { return Simd512Float64(_mm512_cos_pd(a.v)); }
inline Simd512Float64 tan(Simd512Float64 a) { return Simd512Float64(_mm512_tan_pd(a.v)); }
inline Simd512Float64 asin(Simd512Float64 a) { return Simd512Float64(_mm512_asin_pd(a.v)); }
inline Simd512Float64 acos(Simd512Float64 a) { return Simd512Float64(_mm512_acos_pd(a.v)); }
inline Simd512Float64 atan(Simd512Float64 a) { return Simd512Float64(_mm512_atan_pd(a.v)); }
inline Simd512Float64 atan2(Simd512Float64 a, Simd512Float64 b) { return Simd512Float64(_mm512_atan2_pd(a.v, b.v)); }
inline Simd512Float64 sinh(Simd512Float64 a) { return Simd512Float64(_mm512_sinh_pd(a.v)); }
inline Simd512Float64 cosh(Simd512Float64 a) { return Simd512Float64(_mm512_cosh_pd(a.v)); }
inline Simd512Float64 tanh(Simd512Float64 a) { return Simd512Float64(_mm512_tanh_pd(a.v)); }
inline Simd512Float64 asinh(Simd512Float64 a) { return Simd512Float64(_mm512_asinh_pd(a.v)); }
inline Simd512Float64 acosh(Simd512Float64 a) { return Simd512Float64(_mm512_acosh_pd(a.v)); }
inline Simd512Float64 atanh(Simd512Float64 a) { return Simd512Float64(_mm512_atanh_pd(a.v)); }
inline Simd512Float64 abs(Simd512Float64 a) { return Simd512Float64(_mm512_abs_pd(a.v)); }


//Clamp a value between 0.0 and 1.0
[[nodiscard("Value calculated and not used (clamp)")]]
inline Simd512Float64 clamp(const Simd512Float64 a) noexcept {
	const auto zero = _mm512_setzero_pd();
	const auto one = _mm512_setzero_pd();
	return _mm512_min_pd(_mm512_max_pd(a.v, zero), one);
}

//Clamp a value between min and max
[[nodiscard("Value calculated and not used (clamp)")]]
inline Simd512Float64 clamp(const Simd512Float64 a, const Simd512Float64 min, const Simd512Float64 max) noexcept {
	return _mm512_min_pd(_mm512_max_pd(a.v, min.v), max.v);
}

//Clamp a value between min and max
[[nodiscard("Value calculated and not used (clamp)")]]
inline Simd512Float64 clamp(const Simd512Float64 a, const float min_f, const float max_f) noexcept {
	const auto min = _mm512_set1_pd(min_f);
	const auto max = _mm512_set1_pd(max_f);
	return _mm512_min_pd(_mm512_max_pd(a.v, min), max);
}


/**************************************************************************************************
 * SIMD 256 type.  Contains 8 x 64bit Floats
 * Requires AVX support.
 * ************************************************************************************************/
struct Simd256Float64 {
	__m256d v;

	typedef double F;
	typedef Simd256UInt64 U;
	typedef Simd256UInt64 U64;

	Simd256Float64() = default;
	Simd256Float64(__m256d a) : v(a) {}
	Simd256Float64(F a) : v(_mm256_set1_pd(a)) {}

	//*****Support Informtion*****
	static bool cpu_supported(CpuInformation cpuid) {
		return cpuid.has_avx();
	}
	static constexpr int size_of_element() { return sizeof(double); }
	static constexpr int number_of_elements() { return 4; }

	//*****Access Elements*****
	F element(int i) const { return v.m256d_f64[i]; }
	void set_element(int i, F value) { v.m256d_f64[i] = value; }

	//*****Addition Operators*****
	Simd256Float64& operator+=(const Simd256Float64& rhs) noexcept { v = _mm256_add_pd(v, rhs.v); return *this; }
	Simd256Float64& operator+=(double rhs) noexcept { v = _mm256_add_pd(v, _mm256_set1_pd(rhs));	return *this; }

	//*****Subtraction Operators*****
	Simd256Float64& operator-=(const Simd256Float64& rhs) noexcept { v = _mm256_sub_pd(v, rhs.v); return *this; }
	Simd256Float64& operator-=(double rhs) noexcept { v = _mm256_sub_pd(v, _mm256_set1_pd(rhs));	return *this; }

	//*****Multiplication Operators*****
	Simd256Float64& operator*=(const Simd256Float64& rhs) noexcept { v = _mm256_mul_pd(v, rhs.v); return *this; }
	Simd256Float64& operator*=(double rhs) noexcept { v = _mm256_mul_pd(v, _mm256_set1_pd(rhs)); return *this; }

	//*****Division Operators*****
	Simd256Float64& operator/=(const Simd256Float64& rhs) noexcept { v = _mm256_div_pd(v, rhs.v); return *this; }
	Simd256Float64& operator/=(double rhs) noexcept { v = _mm256_div_pd(v, _mm256_set1_pd(rhs));	return *this; }


	//*****Negate Operator*****
	Simd256Float64 operator-() const noexcept { return Simd256Float64(_mm256_sub_pd(_mm256_setzero_pd(), v)); }

	//*****Make Functions****
	static Simd256Float64 make_sequential(F first) { return Simd256Float64(_mm256_set_pd(first + 3.0f, first + 2.0f, first + 1.0f, first)); }
	static Simd256Float64 make_set1(F v) { return _mm256_set1_pd(v); }
	
	//Convert uints that are less than 2^52 to double (this is quicker than full range)
	static Simd256Float64 make_from_uints_52bits(Simd256UInt64 i) {
		//From: https://stackoverflow.com/questions/41144668/how-to-efficiently-perform-double-int64-conversions-with-sse-avx

		auto x = _mm256_and_si256(i.v, _mm256_set1_epi64x(0b0000000000001111111111111111111111111111111111111111111111111111)); //mask of 52-bits.
		x = _mm256_or_si256(x, _mm256_castpd_si256(_mm256_set1_pd(0x0010000000000000)));
		auto u = _mm256_sub_pd(_mm256_castsi256_pd(x), _mm256_set1_pd(0x0010000000000000));
		return Simd256Float64(u);
	}



	//*****Cast Functions****

	//Warning: Requires additional CPU features (AVX2)
	Simd256UInt64 bitcast_to_uint64() const { return Simd256UInt64(_mm256_castpd_si256(this->v)); }

	//*****Approximate Functions*****
	//Returns an approximate reciprocal
	[[nodiscard("Value calculated and not used (reciprocal_approx)")]]
	inline Simd256Float64 reciprocal_approx() const noexcept { return _mm256_div_pd(_mm256_set1_pd(1.0),v); }  //No AVX

};

//*****Addition Operators*****
inline Simd256Float64 operator+(Simd256Float64  lhs, const Simd256Float64& rhs) noexcept { lhs += rhs; return lhs; }
inline Simd256Float64 operator+(Simd256Float64  lhs, double rhs) noexcept { lhs += rhs; return lhs; }
inline Simd256Float64 operator+(double lhs, Simd256Float64 rhs) noexcept { rhs += lhs; return rhs; }

//*****Subtraction Operators*****
inline Simd256Float64 operator-(Simd256Float64  lhs, const Simd256Float64& rhs) noexcept { lhs -= rhs; return lhs; }
inline Simd256Float64 operator-(Simd256Float64  lhs, double rhs) noexcept { lhs -= rhs; return lhs; }
inline Simd256Float64 operator-(const double lhs, const Simd256Float64& rhs) noexcept { return Simd256Float64(_mm256_sub_pd(_mm256_set1_pd(lhs), rhs.v)); }

//*****Multiplication Operators*****
inline Simd256Float64 operator*(Simd256Float64  lhs, const Simd256Float64& rhs) noexcept { lhs *= rhs; return lhs; }
inline Simd256Float64 operator*(Simd256Float64  lhs, double rhs) noexcept { lhs *= rhs; return lhs; }
inline Simd256Float64 operator*(double lhs, Simd256Float64 rhs) noexcept { rhs *= lhs; return rhs; }

//*****Division Operators*****
inline Simd256Float64 operator/(Simd256Float64  lhs, const Simd256Float64& rhs) noexcept { lhs /= rhs;	return lhs; }
inline Simd256Float64 operator/(Simd256Float64  lhs, double rhs) noexcept { lhs /= rhs; return lhs; }
inline Simd256Float64 operator/(const double lhs, const Simd256Float64& rhs) noexcept { return Simd256Float64(_mm256_div_pd(_mm256_set1_pd(lhs), rhs.v)); }

//*****Fused Multiply Add Instructions*****
// Fused Multiply Add (a*b+c)
[[nodiscard("Value calculated and not used (fma)")]]
inline Simd256Float64 fma(const Simd256Float64  a, const Simd256Float64 b, const Simd256Float64 c) {
	return _mm256_fmadd_pd(a.v, b.v, c.v);
}

// Fused Multiply Subtract (a*b-c)
[[nodiscard("Value calculated and not used (fms)")]]
inline Simd256Float64 fms(const Simd256Float64  a, const Simd256Float64 b, const Simd256Float64 c) {
	return _mm256_fmsub_pd(a.v, b.v, c.v);
}

// Fused Negative Multiply Add (-a*b+c)
[[nodiscard("Value calculated and not used (fnma)")]]
inline Simd256Float64 fnma(const Simd256Float64  a, const Simd256Float64 b, const Simd256Float64 c) {
	return _mm256_fnmadd_pd(a.v, b.v, c.v);
}

// Fused Negative Multiply Subtract (-a*b-c)
[[nodiscard("Value calculated and not used (fnms)")]]
inline Simd256Float64 fnms(const Simd256Float64  a, const Simd256Float64 b, const Simd256Float64 c) {
	return _mm256_fnmsub_pd(a.v, b.v, c.v);
}

//*****Rounding Functions*****
inline Simd256Float64 floor(Simd256Float64 a) { return  Simd256Float64(_mm256_floor_pd(a.v)); }
inline Simd256Float64 ceil(Simd256Float64 a) { return  Simd256Float64(_mm256_ceil_pd(a.v)); }
inline Simd256Float64 trunc(Simd256Float64 a) { return  Simd256Float64(_mm256_trunc_pd(a.v)); }
inline Simd256Float64 round(Simd256Float64 a) { return  Simd256Float64(_mm256_round_pd(a.v, _MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC)); }
inline Simd256Float64 fract(Simd256Float64 a) { return a - floor(a); }

//*****Min/Max*****
inline Simd256Float64 min(Simd256Float64 a, Simd256Float64 b) { return Simd256Float64(_mm256_min_pd(a.v, b.v)); }
inline Simd256Float64 max(Simd256Float64 a, Simd256Float64 b) { return Simd256Float64(_mm256_max_pd(a.v, b.v)); }



//*****Approximate Functions*****
inline Simd256Float64 reciprocal_approx(Simd256Float64 a) { return Simd256Float64(1.0 / a); } //No AVX for packed doubles.

//*****Mathematical Functions*****
inline Simd256Float64 sqrt(Simd256Float64 a) { return Simd256Float64(_mm256_sqrt_pd(a.v)); }
inline Simd256Float64 sin(Simd256Float64 a) { return Simd256Float64(_mm256_sin_pd(a.v)); }
inline Simd256Float64 cos(Simd256Float64 a) { return Simd256Float64(_mm256_cos_pd(a.v)); }
inline Simd256Float64 tan(Simd256Float64 a) { return Simd256Float64(_mm256_tan_pd(a.v)); }
inline Simd256Float64 asin(Simd256Float64 a) { return Simd256Float64(_mm256_asin_pd(a.v)); }
inline Simd256Float64 acos(Simd256Float64 a) { return Simd256Float64(_mm256_acos_pd(a.v)); }
inline Simd256Float64 atan(Simd256Float64 a) { return Simd256Float64(_mm256_atan_pd(a.v)); }
inline Simd256Float64 atan2(Simd256Float64 a, Simd256Float64 b) { return Simd256Float64(_mm256_atan2_pd(a.v, b.v)); }
inline Simd256Float64 sinh(Simd256Float64 a) { return Simd256Float64(_mm256_sinh_pd(a.v)); }
inline Simd256Float64 cosh(Simd256Float64 a) { return Simd256Float64(_mm256_cosh_pd(a.v)); }
inline Simd256Float64 tanh(Simd256Float64 a) { return Simd256Float64(_mm256_tanh_pd(a.v)); }
inline Simd256Float64 asinh(Simd256Float64 a) { return Simd256Float64(_mm256_asinh_pd(a.v)); }
inline Simd256Float64 acosh(Simd256Float64 a) { return Simd256Float64(_mm256_acosh_pd(a.v)); }
inline Simd256Float64 atanh(Simd256Float64 a) { return Simd256Float64(_mm256_atanh_pd(a.v)); }
inline Simd256Float64 abs(Simd256Float64 a) {
	auto r = _mm256_and_pd(_mm256_set1_pd(std::bit_cast<float>(0x7FFFFFFF)), a.v); //No AVX for abs
	return Simd256Float64(r);
}

//Clamp a value between 0.0 and 1.0
[[nodiscard("Value calculated and not used (clamp)")]]
inline Simd256Float64 clamp(const Simd256Float64 a) noexcept {
	const auto zero = _mm256_setzero_pd();
	const auto one = _mm256_setzero_pd();
	return _mm256_min_pd(_mm256_max_pd(a.v, zero), one);
}

//Clamp a value between min and max
[[nodiscard("Value calculated and not used (clamp)")]]
inline Simd256Float64 clamp(const Simd256Float64 a, const Simd256Float64 min, const Simd256Float64 max) noexcept {
	return _mm256_min_pd(_mm256_max_pd(a.v, min.v), max.v);
}

//Clamp a value between min and max
[[nodiscard("Value calculated and not used (clamp)")]]
inline Simd256Float64 clamp(const Simd256Float64 a, const float min_f, const float max_f) noexcept {
	const auto min = _mm256_set1_pd(min_f);
	const auto max = _mm256_set1_pd(max_f);
	return _mm256_min_pd(_mm256_max_pd(a.v, min), max);
}

#endif //x86_64





/**************************************************************************************************
 * Check that each type implements the desired types from simd-concepts.h
 * (Not sure why this fails on intelisense, but compliles ok.)
 * ************************************************************************************************/
static_assert(Simd<FallbackFloat64>, "FallbackFloat64 does not implement the concept SIMD");
static_assert(SimdReal<FallbackFloat64>, "FallbackFloat64 does not implement the concept SimdReal");
static_assert(SimdFloat<FallbackFloat64>, "FallbackFloat64 does not implement the concept SimdFloat");
static_assert(SimdFloat64<FallbackFloat64>, "FallbackFloat64 does not implement the concept SimdFloat64");

#if defined(_M_X64) || defined(__x86_64)
static_assert(Simd<Simd256Float64>, "Simd256Float64 does not implement the concept SIMD");
static_assert(Simd<Simd512Float64>, "Simd512Float64 does not implement the concept SIMD");
static_assert(SimdReal<Simd256Float64>, "Simd256Float64 does not implement the concept SimdReal");
static_assert(SimdReal<Simd512Float64>, "Simd512Float64 does not implement the concept SimdReal");
static_assert(SimdFloat<Simd256Float64>, "Simd256Float64 does not implement the concept SimdFloat");
static_assert(SimdFloat<Simd512Float64>, "Simd512Float64 does not implement the concept SimdFloat");
static_assert(SimdFloat64<Simd256Float64>, "Simd256Float64 does not implement the concept SimdFloat64");
static_assert(SimdFloat64<Simd512Float64>, "Simd512Float64 does not implement the concept SimdFloat64");
#endif