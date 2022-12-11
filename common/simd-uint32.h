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

Basic SIMD Types for 32-bit Unsigned Integers.

Each type provides the following operations so they can be used in templated functions.

Support information.
	static bool cpu_supported(CpuInformation cpuid)
	static constexpr int size_of_element()
	static constexpr int number_of_elements()

Operators (rhs same type):
	+=, -=, *=, &=, |=, ^=,
	+,  -,  *,  &,  |,  ^,  ~

Operators (rhs uint32_t)
	+=, -=, *=, /=
	+,  -,  *,  /

Operators (rhs int)
	<< >>


*********************************************************************************************************/
#pragma once

#include <immintrin.h>
#include <stdint.h>

#include "simd-cpuid.h"


/**************************************************************************************************
 * SIMD 512 type.  Contains 8 x 32bit Unsigned Integers
 * Requires AVX-512F 
 * ************************************************************************************************/
struct Simd512UInt32 {
	__m512i v;

	Simd512UInt32(__m512i a) : v(a) {};

	//*****Support Informtion*****
	static bool cpu_supported(CpuInformation cpuid) {
		return cpuid.has_avx512_f();
	}
	static constexpr int size_of_element() { return sizeof(uint32_t); }
	static constexpr int number_of_elements() { return 16; }

	//*****Addition Operators*****
	Simd512UInt32& operator+=(const Simd512UInt32& rhs) noexcept { v = _mm512_add_epi32(v, rhs.v); return *this; }
	Simd512UInt32& operator+=(uint32_t rhs) noexcept { v = _mm512_add_epi32(v, _mm512_set1_epi32(rhs));	return *this; }

	//*****Subtraction Operators*****
	Simd512UInt32& operator-=(const Simd512UInt32& rhs) noexcept { v = _mm512_sub_epi32(v, rhs.v); return *this; }
	Simd512UInt32& operator-=(uint32_t rhs) noexcept { v = _mm512_sub_epi32(v, _mm512_set1_epi32(rhs));	return *this; }

	//*****Multiplication Operators*****
	Simd512UInt32& operator*=(const Simd512UInt32& rhs) noexcept { v = _mm512_mullo_epi32(v, rhs.v); return *this; }
	Simd512UInt32& operator*=(uint32_t rhs) noexcept { v = _mm512_mullo_epi32(v, _mm512_set1_epi32(rhs)); return *this; }

	//*****Division Operators*****
	Simd512UInt32& operator/=(const Simd512UInt32& rhs) noexcept { v = _mm512_div_epu32(v, rhs.v); return *this; }
	Simd512UInt32& operator/=(uint32_t rhs) noexcept { v = _mm512_div_epu32(v, _mm512_set1_epi32(rhs));	return *this; }

	//*****Bitwise Logic Operators*****
	Simd512UInt32& operator&=(const Simd512UInt32& rhs) noexcept { v = _mm512_and_si512(v, rhs.v); return *this; }
	Simd512UInt32& operator|=(const Simd512UInt32& rhs) noexcept { v = _mm512_or_si512(v, rhs.v); return *this; }
	Simd512UInt32& operator^=(const Simd512UInt32& rhs) noexcept { v = _mm512_xor_si512(v, rhs.v); return *this; }

};

//*****Addition Operators*****
inline Simd512UInt32 operator+(Simd512UInt32  lhs, const Simd512UInt32& rhs) noexcept { lhs += rhs; return lhs; }
inline Simd512UInt32 operator+(Simd512UInt32  lhs, uint32_t rhs) noexcept { lhs += rhs; return lhs; }
inline Simd512UInt32 operator+(uint32_t lhs, Simd512UInt32 rhs) noexcept { rhs += lhs; return rhs; }

//*****Subtraction Operators*****
inline Simd512UInt32 operator-(Simd512UInt32  lhs, const Simd512UInt32& rhs) noexcept { lhs -= rhs; return lhs; }
inline Simd512UInt32 operator-(Simd512UInt32  lhs, uint32_t rhs) noexcept { lhs -= rhs; return lhs; }
inline Simd512UInt32 operator-(const uint32_t lhs, const Simd512UInt32& rhs) noexcept { return Simd512UInt32(_mm512_sub_epi32(_mm512_set1_epi32(lhs), rhs.v)); }

//*****Multiplication Operators*****
inline Simd512UInt32 operator*(Simd512UInt32  lhs, const Simd512UInt32& rhs) noexcept { lhs *= rhs; return lhs; }
inline Simd512UInt32 operator*(Simd512UInt32  lhs, uint32_t rhs) noexcept { lhs *= rhs; return lhs; }
inline Simd512UInt32 operator*(uint32_t lhs, Simd512UInt32 rhs) noexcept { rhs *= lhs; return rhs; }

//*****Division Operators*****
inline Simd512UInt32 operator/(Simd512UInt32  lhs, const Simd512UInt32& rhs) noexcept { lhs /= rhs;	return lhs; }
inline Simd512UInt32 operator/(Simd512UInt32  lhs, uint32_t rhs) noexcept { lhs /= rhs; return lhs; }
inline Simd512UInt32 operator/(const uint32_t lhs, const Simd512UInt32& rhs) noexcept { return Simd512UInt32(_mm512_div_epi32(_mm512_set1_epi32(lhs), rhs.v)); }


//*****Bitwise Logic Operators*****
inline Simd512UInt32 operator&(Simd512UInt32  lhs, const Simd512UInt32& rhs) noexcept { lhs &= rhs; return lhs; }
inline Simd512UInt32 operator|(Simd512UInt32  lhs, const Simd512UInt32& rhs) noexcept { lhs |= rhs; return lhs; }
inline Simd512UInt32 operator^(Simd512UInt32  lhs, const Simd512UInt32& rhs) noexcept { lhs ^= rhs; return lhs; }
inline Simd512UInt32 operator~(const Simd512UInt32& lhs) noexcept { return Simd512UInt32(_mm512_xor_epi32(lhs.v, _mm512_set1_epi32(0xFFFFFFFF))); } //No bitwise not in AVX512 so we use xor with a constant..


//*****Shifting Operators*****
inline Simd512UInt32 operator<<(const Simd512UInt32& lhs, int bits) noexcept { return Simd512UInt32(_mm512_slli_epi32(lhs.v, bits)); }
inline Simd512UInt32 operator>>(const Simd512UInt32& lhs, int bits) noexcept { return Simd512UInt32(_mm512_srli_epi32(lhs.v, bits)); }

//*****Min/Max*****
inline Simd512UInt32 min(Simd512UInt32 a, Simd512UInt32 b) { return Simd512UInt32(_mm512_min_epu32(a.v, b.v)); }
inline Simd512UInt32 max(Simd512UInt32 a, Simd512UInt32 b) { return Simd512UInt32(_mm512_max_epu32(a.v, b.v)); }


/**************************************************************************************************
 * SIMD 256 type.  Contains 4 x 32bit Unsigned Integers
 * Requires AVX2 support.
 * ************************************************************************************************/
struct Simd256UInt32 {
	__m256i v;

	Simd256UInt32(__m256i a) : v(a) {};

	//*****Support Informtion*****
	static bool cpu_supported(CpuInformation cpuid) {
		return cpuid.has_avx() && cpuid.has_avx2();
	}
	static constexpr int size_of_element() { return sizeof(uint32_t); }
	static constexpr int number_of_elements() { return 8; }


	//*****Addition Operators*****
	Simd256UInt32& operator+=(const Simd256UInt32& rhs) noexcept { v = _mm256_add_epi32(v, rhs.v); return *this; }
	Simd256UInt32& operator+=(uint32_t rhs) noexcept { v = _mm256_add_epi32(v, _mm256_set1_epi32(rhs));	return *this; }

	//*****Subtraction Operators*****
	Simd256UInt32& operator-=(const Simd256UInt32& rhs) noexcept { v = _mm256_sub_epi32(v, rhs.v); return *this; }
	Simd256UInt32& operator-=(uint32_t rhs) noexcept { v = _mm256_sub_epi32(v, _mm256_set1_epi32(rhs));	return *this; }

	//*****Multiplication Operators*****
	Simd256UInt32& operator*=(const Simd256UInt32& rhs) noexcept {v = _mm256_mullo_epi32(v, rhs.v);	return *this; }

	Simd256UInt32& operator*=(uint32_t rhs) noexcept { *this *= Simd256UInt32(_mm256_set1_epi32(rhs)); return *this; }

	//*****Division Operators*****
	Simd256UInt32& operator/=(const Simd256UInt32& rhs) noexcept { v = _mm256_div_epu32(v, rhs.v); return *this; }
	Simd256UInt32& operator/=(uint32_t rhs) noexcept { v = _mm256_div_epu32(v, _mm256_set1_epi32(rhs));	return *this; }

	//*****Bitwise Logic Operators*****
	Simd256UInt32& operator&=(const Simd256UInt32& rhs) noexcept { v = _mm256_and_si256(v, rhs.v); return *this; }
	Simd256UInt32& operator|=(const Simd256UInt32& rhs) noexcept { v = _mm256_or_si256(v, rhs.v); return *this; }
	Simd256UInt32& operator^=(const Simd256UInt32& rhs) noexcept { v = _mm256_xor_si256(v, rhs.v); return *this; }


};

//*****Addition Operators*****
inline Simd256UInt32 operator+(Simd256UInt32  lhs, const Simd256UInt32& rhs) noexcept { lhs += rhs; return lhs; }
inline Simd256UInt32 operator+(Simd256UInt32  lhs, uint32_t rhs) noexcept { lhs += rhs; return lhs; }
inline Simd256UInt32 operator+(uint32_t lhs, Simd256UInt32 rhs) noexcept { rhs += lhs; return rhs; }

//*****Subtraction Operators*****
inline Simd256UInt32 operator-(Simd256UInt32  lhs, const Simd256UInt32& rhs) noexcept { lhs -= rhs; return lhs; }
inline Simd256UInt32 operator-(Simd256UInt32  lhs, uint32_t rhs) noexcept { lhs -= rhs; return lhs; }
inline Simd256UInt32 operator-(const uint32_t lhs, const Simd256UInt32& rhs) noexcept { return Simd256UInt32(_mm256_sub_epi32(_mm256_set1_epi32(lhs), rhs.v)); }

//*****Multiplication Operators*****
inline Simd256UInt32 operator*(Simd256UInt32  lhs, const Simd256UInt32& rhs) noexcept { lhs *= rhs; return lhs; }
inline Simd256UInt32 operator*(Simd256UInt32  lhs, uint32_t rhs) noexcept { lhs *= rhs; return lhs; }
inline Simd256UInt32 operator*(uint32_t lhs, Simd256UInt32 rhs) noexcept { rhs *= lhs; return rhs; }

//*****Division Operators*****
inline Simd256UInt32 operator/(Simd256UInt32  lhs, const Simd256UInt32& rhs) noexcept { lhs /= rhs;	return lhs; }
inline Simd256UInt32 operator/(Simd256UInt32  lhs, uint32_t rhs) noexcept { lhs /= rhs; return lhs; }
inline Simd256UInt32 operator/(const uint32_t lhs, const Simd256UInt32& rhs) noexcept { return Simd256UInt32(_mm256_div_epi32(_mm256_set1_epi32(lhs), rhs.v)); }


//*****Bitwise Logic Operators*****
inline Simd256UInt32 operator&(Simd256UInt32  lhs, const Simd256UInt32& rhs) noexcept { lhs &= rhs; return lhs; }
inline Simd256UInt32 operator|(Simd256UInt32  lhs, const Simd256UInt32& rhs) noexcept { lhs |= rhs; return lhs; }
inline Simd256UInt32 operator^(Simd256UInt32  lhs, const Simd256UInt32& rhs) noexcept { lhs ^= rhs; return lhs; }
inline Simd256UInt32 operator~(const Simd256UInt32& lhs) noexcept { return Simd256UInt32(_mm256_xor_si256(lhs.v, _mm256_set1_epi32(0xFFFFFFFF))); } //No bitwise not in AVX2 so we use xor with a constant..


//*****Shifting Operators*****
inline Simd256UInt32 operator<<(const Simd256UInt32& lhs, int bits) noexcept { return Simd256UInt32(_mm256_slli_epi32(lhs.v, bits)); }
inline Simd256UInt32 operator>>(const Simd256UInt32& lhs, int bits) noexcept { return Simd256UInt32(_mm256_srli_epi32(lhs.v, bits)); }


//*****Min/Max*****
inline Simd256UInt32 min(Simd256UInt32 a, Simd256UInt32 b) { return Simd256UInt32(_mm256_min_epu32(a.v, b.v)); }
inline Simd256UInt32 max(Simd256UInt32 a, Simd256UInt32 b) { return Simd256UInt32(_mm256_max_epu32(a.v, b.v)); }





/**************************************************************************************************
* Fallback I32 type.
* Contains 1 x 32bit Unsigned Integers
*
* This is a fallback for cpus that are not yet supported.
*
* It may be useful to evaluate a single value from the same code base, as it offers the same interface
* as the SIMD types.
*
* ************************************************************************************************/
struct FallbackUInt32 {
	uint32_t v;

	FallbackUInt32(uint32_t a) : v(a) {};

	//*****Support Informtion*****
	static bool cpu_supported(CpuInformation cpuid) {
		return true;
	}
	static constexpr int size_of_element() { return sizeof(uint32_t); }
	static constexpr int number_of_elements() { return 1; }

	//*****Addition Operators*****
	FallbackUInt32& operator+=(const FallbackUInt32& rhs) noexcept { v += rhs.v; return *this; }
	FallbackUInt32& operator+=(uint32_t rhs) noexcept { v += rhs; return *this; }

	//*****Subtraction Operators*****
	FallbackUInt32& operator-=(const FallbackUInt32& rhs) noexcept { v -= rhs.v; return *this; }
	FallbackUInt32& operator-=(uint32_t rhs) noexcept { v -= rhs; return *this; }

	//*****Multiplication Operators*****
	FallbackUInt32& operator*=(const FallbackUInt32& rhs) noexcept { v *= rhs.v; return *this; }
	FallbackUInt32& operator*=(uint32_t rhs) noexcept { v *= rhs; return *this; }

	//*****Division Operators*****
	FallbackUInt32& operator/=(const FallbackUInt32& rhs) noexcept { v /= rhs.v; return *this; }
	FallbackUInt32& operator/=(uint32_t rhs) noexcept { v /= rhs;	return *this; }

	//*****Bitwise Logic Operators*****
	FallbackUInt32& operator&=(const FallbackUInt32& rhs) noexcept { v &= rhs.v; return *this; }
	FallbackUInt32& operator|=(const FallbackUInt32& rhs) noexcept { v |= rhs.v; return *this; }
	FallbackUInt32& operator^=(const FallbackUInt32& rhs) noexcept { v ^= rhs.v; return *this; }

};

//*****Addition Operators*****
inline FallbackUInt32 operator+(FallbackUInt32  lhs, const FallbackUInt32& rhs) noexcept { lhs += rhs; return lhs; }


inline FallbackUInt32 operator+(FallbackUInt32  lhs, uint32_t rhs) noexcept { lhs += rhs; return lhs; }
inline FallbackUInt32 operator+(uint32_t lhs, FallbackUInt32 rhs) noexcept { rhs += lhs; return rhs; }

//*****Subtraction Operators*****
inline FallbackUInt32 operator-(FallbackUInt32  lhs, const FallbackUInt32& rhs) noexcept { lhs -= rhs; return lhs; }
inline FallbackUInt32 operator-(FallbackUInt32  lhs, uint32_t rhs) noexcept { lhs -= rhs; return lhs; }
inline FallbackUInt32 operator-(const uint32_t lhs, FallbackUInt32 rhs) noexcept { rhs.v = lhs - rhs.v; return rhs; }

//*****Multiplication Operators*****
inline FallbackUInt32 operator*(FallbackUInt32  lhs, const FallbackUInt32& rhs) noexcept { lhs *= rhs; return lhs; }
inline FallbackUInt32 operator*(FallbackUInt32  lhs, uint32_t rhs) noexcept { lhs *= rhs; return lhs; }
inline FallbackUInt32 operator*(uint32_t lhs, FallbackUInt32 rhs) noexcept { rhs *= lhs; return rhs; }

//*****Division Operators*****
inline FallbackUInt32 operator/(FallbackUInt32  lhs, const FallbackUInt32& rhs) noexcept { lhs /= rhs;	return lhs; }
inline FallbackUInt32 operator/(FallbackUInt32  lhs, uint32_t rhs) noexcept { lhs /= rhs; return lhs; }
inline FallbackUInt32 operator/(const uint32_t lhs, FallbackUInt32 rhs) noexcept { rhs.v = lhs / rhs.v; return rhs; }


//*****Bitwise Logic Operators*****
inline FallbackUInt32 operator&(const FallbackUInt32& lhs, const FallbackUInt32& rhs) noexcept { return FallbackUInt32(lhs.v & rhs.v); }
inline FallbackUInt32 operator|(const FallbackUInt32& lhs, const FallbackUInt32& rhs) noexcept { return FallbackUInt32(lhs.v | rhs.v); }
inline FallbackUInt32 operator^(const FallbackUInt32& lhs, const FallbackUInt32& rhs) noexcept { return FallbackUInt32(lhs.v ^ rhs.v); }
inline FallbackUInt32 operator~(FallbackUInt32 lhs) noexcept { return FallbackUInt32(~lhs.v); }


//*****Shifting Operators*****
inline FallbackUInt32 operator<<(FallbackUInt32 lhs, int bits) noexcept { lhs.v <<= bits; return lhs; }
inline FallbackUInt32 operator>>(FallbackUInt32 lhs, int bits) noexcept { lhs.v >>= bits; return lhs; }

//*****Min/Max*****
inline FallbackUInt32 min(FallbackUInt32 a, FallbackUInt32 b) { return FallbackUInt32(std::min(a.v, b.v)); }
inline FallbackUInt32 max(FallbackUInt32 a, FallbackUInt32 b) { return FallbackUInt32(std::max(a.v, b.v)); }