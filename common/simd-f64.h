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
 * SIMD 512 type.  Contains 16 x 64bit Floats
 * Requires AVX-512F support.
 * ************************************************************************************************/
struct Simd512Float64 {
	__m512d v;

	typedef double F;	
	typedef Simd512UInt64 U;
	typedef Simd512UInt64 U64;

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
	Simd512Float64 operator-() const noexcept { return Simd512Float64(_mm512_sub_pd(_mm512_set1_pd(0.0), v)); }

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





};

//*****Addition Operators*****
inline Simd512Float64 operator+(Simd512Float64  lhs, const Simd512Float64& rhs) noexcept { lhs += rhs; return lhs; }
inline Simd512Float64 operator+(Simd512Float64  lhs, double rhs) noexcept { lhs += rhs; return lhs; }
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
	Simd256Float64 operator-() const noexcept { return Simd256Float64(_mm256_sub_pd(_mm256_set1_pd(0.0), v)); }

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
	

	//*****Access Elements*****
	F element(int) const { return v; }
	void set_element(int, F value) { v = value; }

	//*****Support Informtion*****
	static bool cpu_supported(CpuInformation) {
		return true;
	}
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