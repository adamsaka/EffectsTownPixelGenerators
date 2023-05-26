#pragma once
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

CPUID functions. 

This is x86_64 only.  
(We don't bother supporting x86_32 for SIMD code, those machines will use the fallback interfaces)

*********************************************************************************************************/





//Setup some compiler constants
#if defined(_MSC_VER)
	constexpr bool is_visual_studio = true;
#else
	constexpr bool is_visual_studio = false;
#endif

#if defined(__clang__)
	constexpr bool is_clang = true;
#else
	constexpr bool is_clang = false;
#endif

#if defined(__GNUC__)
	constexpr bool is_gcc = true;
#else
	constexpr bool is_gcc = false;
#endif


#if defined(__EMSCRIPTEN__)
	constexpr bool emsctipten = true;
#else
	constexpr bool is_emsctipten = false;
#endif 


//Check the arhitecture
#if defined(_M_X64) || defined(__x86_64)
	constexpr bool is_x64 = true;
#else
	constexpr bool is_x64 = false;
#endif


//MSVC++ does not define SSE macros 
#if (defined(_MSC_VER ) && defined(_M_X64))
	#if !defined(__SSE__)
		#define __SSE__ 1
	#endif
	#if !defined(__SSE2__)
		#define __SSE2__ 1
	#endif

	// I don't know a way to detect SSE3 and SSE4 and compile time, but it exists if AVX does
	#if defined(__AVX__)
		#define __SSE3__ 1
		#define __SSSE3__ 1
		#define __SSE4_1__ 1
		#define __SSE4_2__ 1
	#endif
#endif

//Setup some constexpr variables that we can use to provide some consistancy with different compilers.

#if defined(__SSE__)
	constexpr bool compiler_has_sse = true;
#else
	constexpr bool compiler_has_sse = false;
#endif

#if defined(__SSE2__)
	constexpr bool compiler_has_sse2 = true;
#else
	constexpr bool compiler_has_sse2 = false;
#endif

#if defined(__SSE3__)
	constexpr bool compiler_has_sse3 = true;
#else
	constexpr bool compiler_has_sse3 = false;
#endif

#if defined(__SSSE3__)
	constexpr bool compiler_has_ssse3 = true;
#else
	constexpr bool compiler_has_ssse3 = false;
#endif

#if defined(__SSE4_1__)
	constexpr bool compiler_has_sse4_1 = true;
#else
	constexpr bool compiler_has_sse4_1 = false;
#endif

#if defined(__SSE4_2__)
	constexpr bool compiler_has_sse4_2 = true;
#else
	constexpr bool compiler_has_sse4_2 = false;
#endif

#if defined(__AVX__)
	constexpr bool compiler_has_avx = true;
#else
	constexpr bool compiler_has_avx = false;
#endif

#if defined(__AVX2__)
	constexpr bool compiler_has_avx2 = true;
#else
	constexpr bool compiler_has_avx2 = false;
#endif

#if defined(__AVX512F__)
	constexpr bool compiler_has_avx512f = true;
#else
	constexpr bool compiler_has_avx512f = false;
#endif

#if defined(__AVX512DQ__)
	constexpr bool compiler_has_avx512dq = true;
#else
	constexpr bool compiler_has_avx512dq = false;
#endif

#if defined(__AVX512VL__)
	constexpr bool compiler_has_avx512vl = true;
#else
	constexpr bool compiler_has_avx512vl = false;
#endif


#if defined(__AVX512BW__)
	constexpr bool compiler_has_avx512bw = true;
#else
	constexpr bool compiler_has_avx512bw = false;
#endif

#if defined(__AVX512CD__)
	constexpr bool compiler_has_avx512cd = true;
#else
	constexpr bool compiler_has_avx512cd = false;
#endif











//This is x86_64 only. 
#if defined(_M_X64) || defined(__x86_64)


#include <stdint.h>
#include <intrin.h>
#include <bitset>
#include <string>

class CpuInformation {
private:
	std::bitset<32> ecx1{}; //ecx from function 1
	std::bitset<32> edx1{}; //edx from function 1
	std::bitset<32> ebx7{}; //ebx from function 7
	std::bitset<32> ecx7{}; //ecx from function 7
	std::bitset<32> edx7{}; //edx from function 7
	std::bitset<32> eax7_1{}; //edx from function 7

public:
	
	//Constructor - Performs CPUIDs and save reults
	CpuInformation() {
		int data[4];

		//Get the number of ids
		__cpuid(data,0);
		int max_id = data[0];

		if (max_id >= 1) {
			__cpuid(data, 1);
			ecx1 = data[2];
			edx1 = data[3];
		}
		if (max_id >= 7) {
			__cpuidex(data, 7, 0);
			ebx7 = data[1];
			ecx7 = data[2];
			edx7 = data[3];			
			
			__cpuidex(data, 7, 1);
			eax7_1 = data[1];
		}
	}

	bool has_sse() const noexcept { return edx1[25]; }
	bool has_sse2() const noexcept { return edx1[25]; }
	bool has_sse3() const noexcept { return ecx1[0]; }
	bool has_ssse3() const noexcept { return ecx1[9]; }
	bool has_sse41() const noexcept { return ecx1[19]; }
	bool has_sse42() const noexcept { return ecx1[20]; }
	bool has_fma() const noexcept { return ecx1[12]; }
	bool has_avx() const noexcept { return ecx1[28]; }
	bool has_f16c() const noexcept { return ecx1[29]; }
	bool has_avx2() const noexcept { return ebx7[5]; }
	bool has_avx512_f() const noexcept { return ebx7[16]; }
	bool has_avx512_dq() const noexcept { return ebx7[17]; }
	bool has_avx512_ifma() const noexcept { return ebx7[21]; }
	bool has_avx512_pf() const noexcept { return ebx7[26]; }
	bool has_avx512_er() const noexcept { return ebx7[27]; }
	bool has_avx512_cd() const noexcept { return ebx7[28]; }
	bool has_sha() const noexcept { return ebx7[29]; }
	bool has_avx512_bw() const noexcept { return ebx7[30]; }
	bool has_avx512_vl() const noexcept { return ebx7[31]; }
	bool has_avx512_vbmi() const noexcept { return ecx7[1]; }
	bool has_avx512_vbmi2() const noexcept { return ecx7[6]; }
	bool has_avx512_gfni() const noexcept { return ecx7[8]; }
	bool has_avx512_vaes() const noexcept { return ecx7[9]; }
	
	bool has_avx512_vpclmulqdq() const noexcept { return ecx7[10]; }
	bool has_avx512_vnni() const noexcept { return ecx7[11]; }
	bool has_avx512_bitalg() const noexcept { return ecx7[12]; }
	bool has_avx512_vpopcntdq() const noexcept { return ecx7[14]; }
	bool has_avx512_4vnniw() const noexcept { return edx7[2]; }
	bool has_avx512_4fmaps() const noexcept { return edx7[3]; }
	bool has_avx512_vp2intersect() const noexcept { return edx7[8]; }
	bool has_avx512_bf16() const noexcept { return eax7_1[5]; }
	bool has_avx512_fp16() const noexcept { return edx7[23]; }


	

	/**************************************************************************************************
	* Checks that the CPU implements x86_64 Microarchitecture level 1
	* See: https://en.wikipedia.org/wiki/X86-64#Microarchitecture_levels
	* ************************************************************************************************/
	bool is_level_1() const noexcept {
		return has_sse2() && has_sse();
	}

	/**************************************************************************************************
	* Checks that the CPU implements x86_64 Microarchitecture level 2
	* See: https://en.wikipedia.org/wiki/X86-64#Microarchitecture_levels
	* ************************************************************************************************/
	bool is_level_2() const noexcept  {
		return  has_sse42() && has_sse41() && has_sse3() && has_ssse3() &&  is_level_1();
	}

	/**************************************************************************************************
	* Checks that the CPU implements x86_64 Microarchitecture level 3
	* See: https://en.wikipedia.org/wiki/X86-64#Microarchitecture_levels
	* ************************************************************************************************/
	bool is_level_3() const noexcept {
		return  has_avx2() && has_fma() && has_avx() &&  has_f16c() && is_level_2();
	}

	/**************************************************************************************************
	* Checks that the CPU implements x86_64 Microarchitecture level 4
	* See: https://en.wikipedia.org/wiki/X86-64#Microarchitecture_levels
	* ************************************************************************************************/
	bool is_level_4() const noexcept {
		return  has_avx512_bw() && has_avx512_cd() && has_avx512_dq() && has_avx512_vl() && has_avx512_f() && is_level_3();
	}


	//Returns a multiline string to show user their supported features.
	std::string to_string(){
		std::string s{};
		s += "Has SSE                 : " + yes_no(has_sse()) + "\n";
		s += "Has SSE2                : " + yes_no(has_sse2()) + "\n";
		s += "Has SSE3                : " + yes_no(has_sse3()) + "\n";
		s += "Has SSE4.1              : " + yes_no(has_sse41()) + "\n";
		s += "Has SSE4.2              : " + yes_no(has_sse42()) + "\n";
		s += "Has FMA                 : " + yes_no(has_fma()) + "\n";
		s += "Has AVX                 : " + yes_no(has_avx()) + "\n";
		s += "Has AVX2                : " + yes_no(has_avx2()) + "\n";
		s += "Has AVX512 F            : " + yes_no(has_avx512_f()) + "\n";
		s += "Has AVX512 CD           : " + yes_no(has_avx512_cd()) + "\n";
		s += "Has AVX512 ER           : " + yes_no(has_avx512_er()) + "\n";
		s += "Has AVX512 PF           : " + yes_no(has_avx512_pf()) + "\n";
		s += "Has AVX512 4FMAPS       : " + yes_no(has_avx512_4fmaps()) + "\n";
		s += "Has AVX512 4VNNIW       : " + yes_no(has_avx512_4vnniw()) + "\n";
		s += "Has AVX512 VPOPCNTDQ    : " + yes_no(has_avx512_vpopcntdq()) + "\n";
		s += "Has AVX512 VL           : " + yes_no(has_avx512_vl()) + "\n";
		s += "Has AVX512 DQ           : " + yes_no(has_avx512_dq()) + "\n";
		s += "Has AVX512 BW           : " + yes_no(has_avx512_bw()) + "\n";
		s += "Has AVX512 IFMA         : " + yes_no(has_avx512_ifma()) + "\n";
		s += "Has AVX512 VNNI         : " + yes_no(has_avx512_vnni()) + "\n";
		s += "Has AVX512 BF16         : " + yes_no(has_avx512_bf16()) + "\n";
		s += "Has AVX512 VBMI2        : " + yes_no(has_avx512_vbmi2()) + "\n";
		s += "Has AVX512 BITALG       : " + yes_no(has_avx512_bitalg()) + "\n";
		s += "Has AVX512 VPCLMULQDQ   : " + yes_no(has_avx512_vpclmulqdq()) + "\n";
		s += "Has AVX512 GFNI         : " + yes_no(has_avx512_gfni()) + "\n";
		s += "Has AVX512 VAES         : " + yes_no(has_avx512_vaes()) + "\n";
		s += "Has AVX512 VP2INTERSECT : " + yes_no(has_avx512_vp2intersect()) + "\n";
		s += "Has AVX512 FP16         : " + yes_no(has_avx512_fp16()) + "\n";
		return s;
	}

	private:
		inline std::string yes_no(bool v) {
			return (v) ? "Yes" : "No";
		}

};

#endif