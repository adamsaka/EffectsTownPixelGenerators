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

Basic SIMD Types for 64-bit Unsigned Integers.

Each type provides the following operations so they can be used in templated functions.

Support information.
	static bool cpu_supported(CpuInformation cpuid)
	static constexpr int size_of_element()
	static constexpr int number_of_elements()

Operators (rhs same type):	
	+=, -=, *=, &=, |=, ^=, 
	+,  -,  *,  &,  |,  ^,  ~

Operators (rhs uint64_t)
	+=, -=, *=, /= 
	+,  -,  *,  / 

Operators (rhs int)
	<< >>


*********************************************************************************************************/
#pragma once

#include <stdint.h>
#include <bit>

#include "simd-cpuid.h"
#include "simd-concepts.h"


/**************************************************************************************************
* Fallback I64 type.
* Contains 1 x 64bit Unsigned Integers
*
* This is a fallback for cpus that are not yet supported.
*
* It may be useful to evaluate a single value from the same code base, as it offers the same interface
* as the SIMD types.
*
* ************************************************************************************************/
struct FallbackUInt64 {
	uint64_t v;
	typedef uint64_t F;

	FallbackUInt64() = default;
	FallbackUInt64(uint64_t a) : v(a) {};

	//*****Support Informtion*****
#if defined(_M_X64) || defined(__x86_64)

	static bool cpu_supported(CpuInformation) {
		return true;
	}
#endif
	static bool cpu_supported() { return true; }

	//*****Elements*****
	static constexpr int size_of_element() { return sizeof(uint64_t); }
	static constexpr int number_of_elements() { return 1; }	
	F element(int) { return v; }

	//*****Make Functions****
	static FallbackUInt64 make_sequential(uint32_t first) { return FallbackUInt64(first); }


	//*****Addition Operators*****
	FallbackUInt64& operator+=(const FallbackUInt64& rhs) noexcept { v += rhs.v; return *this; }
	FallbackUInt64& operator+=(uint64_t rhs) noexcept { v += rhs; return *this; }

	//*****Subtraction Operators*****
	FallbackUInt64& operator-=(const FallbackUInt64& rhs) noexcept { v -= rhs.v; return *this; }
	FallbackUInt64& operator-=(uint64_t rhs) noexcept { v -= rhs; return *this; }

	//*****Multiplication Operators*****
	FallbackUInt64& operator*=(const FallbackUInt64& rhs) noexcept { v *= rhs.v; return *this; }
	FallbackUInt64& operator*=(uint64_t rhs) noexcept { v *= rhs; return *this; }

	//*****Division Operators*****
	FallbackUInt64& operator/=(const FallbackUInt64& rhs) noexcept { v /= rhs.v; return *this; }
	FallbackUInt64& operator/=(uint64_t rhs) noexcept { v /= rhs;	return *this; }

	//*****Bitwise Logic Operators*****
	FallbackUInt64& operator&=(const FallbackUInt64& rhs) noexcept { v &= rhs.v; return *this; }
	FallbackUInt64& operator|=(const FallbackUInt64& rhs) noexcept { v |= rhs.v; return *this; }
	FallbackUInt64& operator^=(const FallbackUInt64& rhs) noexcept { v ^= rhs.v; return *this; }

};

//*****Addition Operators*****
inline FallbackUInt64 operator+(FallbackUInt64  lhs, const FallbackUInt64& rhs) noexcept { lhs += rhs; return lhs; }


inline FallbackUInt64 operator+(FallbackUInt64  lhs, uint64_t rhs) noexcept { lhs += rhs; return lhs; }
inline FallbackUInt64 operator+(uint64_t lhs, FallbackUInt64 rhs) noexcept { rhs += lhs; return rhs; }

//*****Subtraction Operators*****
inline FallbackUInt64 operator-(FallbackUInt64  lhs, const FallbackUInt64& rhs) noexcept { lhs -= rhs; return lhs; }
inline FallbackUInt64 operator-(FallbackUInt64  lhs, uint64_t rhs) noexcept { lhs -= rhs; return lhs; }
inline FallbackUInt64 operator-(const uint64_t lhs, FallbackUInt64 rhs) noexcept { rhs.v = lhs - rhs.v; return rhs; }

//*****Multiplication Operators*****
inline FallbackUInt64 operator*(FallbackUInt64  lhs, const FallbackUInt64& rhs) noexcept { lhs *= rhs; return lhs; }
inline FallbackUInt64 operator*(FallbackUInt64  lhs, uint64_t rhs) noexcept { lhs *= rhs; return lhs; }
inline FallbackUInt64 operator*(uint64_t lhs, FallbackUInt64 rhs) noexcept { rhs *= lhs; return rhs; }

//*****Division Operators*****
inline FallbackUInt64 operator/(FallbackUInt64  lhs, const FallbackUInt64& rhs) noexcept { lhs /= rhs;	return lhs; }
inline FallbackUInt64 operator/(FallbackUInt64  lhs, uint64_t rhs) noexcept { lhs /= rhs; return lhs; }
inline FallbackUInt64 operator/(const uint64_t lhs, FallbackUInt64 rhs) noexcept { rhs.v = lhs / rhs.v; return rhs; }


//*****Bitwise Logic Operators*****
inline FallbackUInt64 operator&(const FallbackUInt64& lhs, const FallbackUInt64& rhs) noexcept { return FallbackUInt64(lhs.v & rhs.v); }
inline FallbackUInt64 operator|(const FallbackUInt64& lhs, const FallbackUInt64& rhs) noexcept { return FallbackUInt64(lhs.v | rhs.v); }
inline FallbackUInt64 operator^(const FallbackUInt64& lhs, const FallbackUInt64& rhs) noexcept { return FallbackUInt64(lhs.v ^ rhs.v); }
inline FallbackUInt64 operator~(FallbackUInt64 lhs) noexcept { return FallbackUInt64(~lhs.v); }


//*****Shifting Operators*****
inline FallbackUInt64 operator<<(FallbackUInt64 lhs, int bits) noexcept { lhs.v <<= bits; return lhs; }
inline FallbackUInt64 operator>>(FallbackUInt64 lhs, int bits) noexcept { lhs.v >>= bits; return lhs; }

inline FallbackUInt64 rotl(const FallbackUInt64& a, int bits) { return std::rotl(a.v, bits); };
inline FallbackUInt64 rotr(const FallbackUInt64& a, int bits) { return std::rotr(a.v, bits); };

//*****Min/Max*****
inline FallbackUInt64 min(FallbackUInt64 a, FallbackUInt64 b) { return FallbackUInt64(std::min(a.v, b.v)); }
inline FallbackUInt64 max(FallbackUInt64 a, FallbackUInt64 b) { return FallbackUInt64(std::max(a.v, b.v)); }



//***************** x86_64 only code ******************
#if defined(_M_X64) || defined(__x86_64)
#include <immintrin.h>


/**************************************************************************************************
 * SIMD 512 type.  Contains 8 x 64bit Unsigned Integers
 * Requires AVX-512F and AVX-512DQ support.
 * ************************************************************************************************/
struct Simd512UInt64 {
	__m512i v;
	typedef uint64_t F;

	Simd512UInt64() = default;
	Simd512UInt64(__m512i a) : v(a) {};
	Simd512UInt64(F a) : v(_mm512_set1_epi64(a)) {}

	//*****Elements*****
	F element(int i) { return v.m512i_u64[i]; }

	//*****Support Informtion*****
	static bool cpu_supported() {
		CpuInformation cpuid{};
		cpu_supported(cpuid);
	}
	static bool cpu_supported(CpuInformation cpuid) {
		return cpuid.has_avx512_f() && cpuid.has_avx512_dq();
	}
	static constexpr int size_of_element() { return sizeof(uint64_t); }
	static constexpr int number_of_elements() { return 8; }

	//*****Make Functions****
	static Simd512UInt64 make_sequential(uint64_t first) { return Simd512UInt64(_mm512_set_epi64(first + 7, first + 6, first + 5, first + 4, first + 3, first + 2, first + 1, first)); }


	//*****Addition Operators*****
	Simd512UInt64& operator+=(const Simd512UInt64& rhs) noexcept { v = _mm512_add_epi64(v, rhs.v); return *this; }
	Simd512UInt64& operator+=(uint64_t rhs) noexcept { v = _mm512_add_epi64(v, _mm512_set1_epi64(rhs));	return *this; }

	//*****Subtraction Operators*****
	Simd512UInt64& operator-=(const Simd512UInt64& rhs) noexcept { v = _mm512_sub_epi64(v, rhs.v); return *this; }
	Simd512UInt64& operator-=(uint64_t rhs) noexcept { v = _mm512_sub_epi64(v, _mm512_set1_epi64(rhs));	return *this; }

	//*****Multiplication Operators*****
	Simd512UInt64& operator*=(const Simd512UInt64& rhs) noexcept { v = _mm512_mullo_epi64(v, rhs.v); return *this; }
	Simd512UInt64& operator*=(uint64_t rhs) noexcept { v = _mm512_mullo_epi64(v, _mm512_set1_epi64(rhs)); return *this; }

	//*****Division Operators*****
	Simd512UInt64& operator/=(const Simd512UInt64& rhs) noexcept { v = _mm512_div_epu64(v, rhs.v); return *this; }
	Simd512UInt64& operator/=(uint64_t rhs) noexcept { v = _mm512_div_epu64(v, _mm512_set1_epi64(rhs));	return *this; }

	//*****Bitwise Logic Operators*****
	Simd512UInt64& operator&=(const Simd512UInt64& rhs) noexcept {v= _mm512_and_si512(v, rhs.v); return *this; }
	Simd512UInt64& operator|=(const Simd512UInt64& rhs) noexcept {v= _mm512_or_si512(v, rhs.v); return *this; }
	Simd512UInt64& operator^=(const Simd512UInt64& rhs) noexcept {v= _mm512_xor_si512(v, rhs.v); return *this; }



};

//*****Addition Operators*****
inline Simd512UInt64 operator+(Simd512UInt64  lhs, const Simd512UInt64& rhs) noexcept { lhs += rhs; return lhs; }
inline Simd512UInt64 operator+(Simd512UInt64  lhs, uint64_t rhs) noexcept { lhs += rhs; return lhs; }
inline Simd512UInt64 operator+(uint64_t lhs, Simd512UInt64 rhs) noexcept { rhs += lhs; return rhs; }

//*****Subtraction Operators*****
inline Simd512UInt64 operator-(Simd512UInt64  lhs, const Simd512UInt64& rhs) noexcept { lhs -= rhs; return lhs; }
inline Simd512UInt64 operator-(Simd512UInt64  lhs, uint64_t rhs) noexcept { lhs -= rhs; return lhs; }
inline Simd512UInt64 operator-(const uint64_t lhs, const Simd512UInt64& rhs) noexcept { return Simd512UInt64(_mm512_sub_epi64(_mm512_set1_epi64(lhs), rhs.v)); }

//*****Multiplication Operators*****
inline Simd512UInt64 operator*(Simd512UInt64  lhs, const Simd512UInt64& rhs) noexcept { lhs *= rhs; return lhs; }
inline Simd512UInt64 operator*(Simd512UInt64  lhs, uint64_t rhs) noexcept { lhs *= rhs; return lhs; }
inline Simd512UInt64 operator*(uint64_t lhs, Simd512UInt64 rhs) noexcept { rhs *= lhs; return rhs; }

//*****Division Operators*****
inline Simd512UInt64 operator/(Simd512UInt64  lhs, const Simd512UInt64& rhs) noexcept { lhs /= rhs;	return lhs; }
inline Simd512UInt64 operator/(Simd512UInt64  lhs, uint64_t rhs) noexcept { lhs /= rhs; return lhs; }
inline Simd512UInt64 operator/(const uint64_t lhs, const Simd512UInt64& rhs) noexcept { return Simd512UInt64(_mm512_div_epi64(_mm512_set1_epi64(lhs), rhs.v)); }


//*****Bitwise Logic Operators*****
inline Simd512UInt64 operator&(Simd512UInt64  lhs, const Simd512UInt64& rhs) noexcept { lhs &= rhs; return lhs; }
inline Simd512UInt64 operator|(Simd512UInt64  lhs, const Simd512UInt64& rhs) noexcept { lhs |= rhs; return lhs; }
inline Simd512UInt64 operator^(Simd512UInt64  lhs, const Simd512UInt64& rhs) noexcept { lhs ^= rhs; return lhs; }
inline Simd512UInt64 operator~(const Simd512UInt64& lhs) noexcept { return Simd512UInt64(_mm512_xor_epi64(lhs.v, _mm512_set1_epi64(0xFFFFFFFFFFFFFFFF))); } //No bitwise not in AVX512 so we use xor with a constant..


//*****Shifting Operators*****
inline Simd512UInt64 operator<<(const Simd512UInt64& lhs, int bits) noexcept { return Simd512UInt64(_mm512_slli_epi64(lhs.v, bits)); }
inline Simd512UInt64 operator>>(const Simd512UInt64& lhs, int bits) noexcept { return Simd512UInt64(_mm512_srli_epi64(lhs.v, bits)); }

inline Simd512UInt64 rotl(const Simd512UInt64& a, int bits) { return a << bits | a >> (64 - bits); };
inline Simd512UInt64 rotr(const Simd512UInt64& a, int bits) { return a >> bits | a << (64 - bits); };

//*****Min/Max*****
inline Simd512UInt64 min(Simd512UInt64 a, Simd512UInt64 b) { return Simd512UInt64(_mm512_min_epu64(a.v, b.v)); }
inline Simd512UInt64 max(Simd512UInt64 a, Simd512UInt64 b) { return Simd512UInt64(_mm512_max_epu64(a.v, b.v)); }



/**************************************************************************************************
 * SIMD 256 type.  Contains 4 x 64bit Unsigned Integers
 * Requires AVX2 support.
 * ************************************************************************************************/
struct Simd256UInt64 {
	__m256i v;

	typedef uint64_t F;

	Simd256UInt64() = default;
	Simd256UInt64(__m256i a) : v(a) {};
	Simd256UInt64(F a) : v(_mm256_set1_epi64x(a)) {}

	//*****Support Informtion*****
	static bool cpu_supported() {
		CpuInformation cpuid{};
		cpu_supported(cpuid);
	}
	static bool cpu_supported(CpuInformation cpuid) {
		return cpuid.has_avx() && cpuid.has_avx2();
	}
	static constexpr int size_of_element() { return sizeof(uint64_t); }
	static constexpr int number_of_elements() { return 4; }
	
	//*****Elements*****
	F element(int i) { return v.m256i_u64[i]; }


	//*****Addition Operators*****
	Simd256UInt64& operator+=(const Simd256UInt64& rhs) noexcept {v = _mm256_add_epi64(v, rhs.v); return *this;}
	Simd256UInt64& operator+=(const uint64_t rhs) noexcept {v = _mm256_add_epi64(v, _mm256_set1_epi64x(rhs));	return *this;}

	//*****Subtraction Operators*****
	Simd256UInt64& operator-=(const Simd256UInt64& rhs) noexcept {v = _mm256_sub_epi64(v, rhs.v); return *this;}
	Simd256UInt64& operator-=(const uint64_t rhs) noexcept {v = _mm256_sub_epi64(v, _mm256_set1_epi64x(rhs));	return *this; }

	//*****Multiplication Operators*****
	Simd256UInt64& operator*=(const Simd256UInt64& rhs) noexcept {
		if constexpr (mt::environment::compiler_has_avx512vl && mt::environment::compiler_has_avx512dq) {
			//There is a slight chance a user is using this struct but has compiled the code with avx512dq and avx512vl enabled.
			this->v = _mm256_mullo_epi64(v, rhs.v);											
			return *this;
		}
		else {
			/*
			Multiplication (i64*i64->i64) is not implemented for AVX2.
			Algorithm:
					If x*y = [a,b]*[c,d] (where a,b,c,d are digits/dwords).
						   = (2^64) ac + (2^32) (ad + bc) + bd
							 (We ignore upper 64bits so, we can ignore the (2^64) term.)
			*/
			//Given lhs = [a,b] and rhs=[c,d]
			auto digit1 = _mm256_mul_epu32(v, rhs.v);											//=[0+carry, bd]	: Calculate bd (i32*i32->i64)
			auto rhs_swap = _mm256_shuffle_epi32(rhs.v, 0xB1);									//=[d,c]			: Swaps the low and high dwords on RHS . 
			auto ad_bc = _mm256_mullo_epi32(v, rhs_swap);										//=[ad,bc]			: Multiply every dword together (i32*i32->i32).          
			auto bc_00 = _mm256_slli_epi64(ad_bc, 32);											//=[bc,0]			: Shift Left to put bc in the upper dword.
			auto ad_plus_bc = _mm256_add_epi32(ad_bc, bc_00);									//=[ad+bc,bc]		: Perofrm addition in the upper dword
			auto digit2 = _mm256_and_si256(ad_plus_bc, _mm256_set1_epi64x(0xFFFFFFFF00000000)); //=[ad+bc,0]		: Zero lower dword using &
			this->v = _mm256_add_epi64(digit1, digit2);											//=[ad+dc+carry,bd] : Add digits to get final result. 
			return *this;
		}
	}

	Simd256UInt64& operator*=(uint64_t rhs) noexcept { *this *= Simd256UInt64(_mm256_set1_epi64x(rhs)); return *this; }

	//*****Division Operators*****
	Simd256UInt64& operator/=(const Simd256UInt64& rhs) noexcept { v = _mm256_div_epu64(v, rhs.v); return *this; }
	Simd256UInt64& operator/=(uint64_t rhs) noexcept { v = _mm256_div_epu64(v, _mm256_set1_epi64x(rhs));	return *this; }

	//*****Bitwise Logic Operators*****
	Simd256UInt64& operator&=(const Simd256UInt64& rhs) noexcept {v=_mm256_and_si256(v, rhs.v);return *this;}
	Simd256UInt64& operator|=(const Simd256UInt64& rhs) noexcept {v=_mm256_or_si256(v, rhs.v); return *this;}
	Simd256UInt64& operator^=(const Simd256UInt64&rhs) noexcept {v=_mm256_xor_si256(v, rhs.v);return *this;}

	//*****Make Functions****
	static Simd256UInt64 make_sequential(uint64_t first) noexcept { return Simd256UInt64(_mm256_set_epi64x(first + 3, first + 2, first + 1, first)); }
	


};

//*****Addition Operators*****
inline Simd256UInt64 operator+(Simd256UInt64  lhs, const Simd256UInt64& rhs) noexcept { lhs += rhs; return lhs; }
inline Simd256UInt64 operator+(Simd256UInt64  lhs, uint64_t rhs) noexcept { lhs += rhs; return lhs; }
inline Simd256UInt64 operator+(uint64_t lhs, Simd256UInt64 rhs) noexcept { rhs += lhs; return rhs; }

//*****Subtraction Operators*****
inline Simd256UInt64 operator-(Simd256UInt64  lhs, const Simd256UInt64& rhs) noexcept { lhs -= rhs; return lhs; }
inline Simd256UInt64 operator-(Simd256UInt64  lhs, uint64_t rhs) noexcept { lhs -= rhs; return lhs; }
inline Simd256UInt64 operator-(const uint64_t lhs, const Simd256UInt64&  rhs) noexcept { return Simd256UInt64(_mm256_sub_epi64(_mm256_set1_epi64x(lhs), rhs.v)); }

//*****Multiplication Operators*****
inline Simd256UInt64 operator*(Simd256UInt64  lhs, const Simd256UInt64& rhs) noexcept { lhs *= rhs; return lhs; }
inline Simd256UInt64 operator*(Simd256UInt64  lhs, uint64_t rhs) noexcept { lhs *= rhs; return lhs; }
inline Simd256UInt64 operator*(uint64_t lhs, Simd256UInt64 rhs) noexcept { rhs *= lhs; return rhs; }

//*****Division Operators*****
inline Simd256UInt64 operator/(Simd256UInt64  lhs, const Simd256UInt64 & rhs) noexcept { lhs /= rhs;	return lhs; }
inline Simd256UInt64 operator/(Simd256UInt64  lhs, uint64_t rhs) noexcept {lhs /= rhs; return lhs; }
inline Simd256UInt64 operator/(const uint64_t lhs, const Simd256UInt64& rhs) noexcept { return Simd256UInt64(_mm256_div_epi64(_mm256_set1_epi64x(lhs), rhs.v)); }


//*****Bitwise Logic Operators*****
inline Simd256UInt64 operator&(Simd256UInt64  lhs, const Simd256UInt64& rhs) noexcept { lhs &= rhs; return lhs; }
inline Simd256UInt64 operator|(Simd256UInt64  lhs, const Simd256UInt64& rhs) noexcept { lhs |= rhs; return lhs; }
inline Simd256UInt64 operator^(Simd256UInt64  lhs, const Simd256UInt64& rhs) noexcept { lhs ^= rhs; return lhs; }
inline Simd256UInt64 operator~(const Simd256UInt64& lhs) noexcept { return Simd256UInt64(_mm256_xor_si256(lhs.v, _mm256_set1_epi64x(0xFFFFFFFFFFFFFFFF))); } //No bitwise not in AVX2 so we use xor with a constant..
	

//*****Shifting Operators*****
inline Simd256UInt64 operator<<(const Simd256UInt64& lhs, int bits) noexcept { return Simd256UInt64(_mm256_slli_epi64(lhs.v, bits)); }
inline Simd256UInt64 operator>>(const Simd256UInt64& lhs, int bits) noexcept { return Simd256UInt64(_mm256_srli_epi64(lhs.v, bits)); }

inline Simd256UInt64 rotl(const Simd256UInt64& a, int bits) noexcept { return a << bits | a >> (64 - bits); };
inline Simd256UInt64 rotr(const Simd256UInt64& a, int bits) noexcept { return a >> bits | a << (64 - bits); };

//*****Min/Max*****
inline Simd256UInt64 min(Simd256UInt64 a, Simd256UInt64 b) noexcept { return Simd256UInt64(_mm256_min_epu64(a.v, b.v)); }
inline Simd256UInt64 max(Simd256UInt64 a, Simd256UInt64 b) noexcept { return Simd256UInt64(_mm256_max_epu64(a.v, b.v)); }


#endif //x86_64




/**************************************************************************************************
 * Check that each type implements the desired types from simd-concepts.h
 * (Not sure why this fails on intelisense, but compliles ok.)
 * ************************************************************************************************/
static_assert(Simd<FallbackUInt64>, "FallbackUInt64 does not implement the concept Simd");
static_assert(SimdUInt<FallbackUInt64>, "FallbackUInt64 does not implement the concept SimdUInt");
static_assert(SimdUInt64<FallbackUInt64>, "FallbackUInt64 does not implement the concept SimdUInt64");



#if defined(_M_X64) || defined(__x86_64)
static_assert(Simd<Simd256UInt64>, "Simd256UInt64 does not implement the concept Simd");
static_assert(Simd<Simd512UInt64>, "Simd512UInt64 does not implement the concept Simd");
static_assert(SimdUInt<Simd256UInt64>, "Simd256UInt64 does not implement the concept SimdUInt");
static_assert(SimdUInt<Simd512UInt64>, "Simd512UInt64 does not implement the concept SimdUInt");
static_assert(SimdUInt64<Simd256UInt64>, "Simd256UInt64 does not implement the concept SimdUInt64");
static_assert(SimdUInt64<Simd512UInt64>, "Simd512UInt64 does not implement the concept SimdUInt64");
#endif


/**************************************************************************************************
 Define SimdNativeUInt64 as the best supported type at compile time.
*************************************************************************************************/
#if defined(_M_X64) || defined(__x86_64)
#if defined(__AVX512F__) && defined(__AVX512DQ__) 
typedef Simd512UInt64 SimdNativeUInt64;
#else
#if defined(__AVX2__) && defined(__AVX__) 
typedef Simd256UInt64 SimdNativeUInt64;
#else
#if defined(__SSE4_1__) && defined(__SSE4_1__) && defined(__SSE3__) && defined(__SSSE3__) 
typedef FallbackUInt64 SimdNativeUInt64;
#else
typedef FallbackUInt64 SimdNativeUInt64;
#endif	
#endif	
#endif
#else
typedef FallbackUInt64 typedef FallbackUInt64 SimdNativeUInt64;
#endif