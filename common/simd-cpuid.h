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

*********************************************************************************************************/
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

	bool has_sse() { return edx1[25]; }
	bool has_sse2() { return edx1[25]; }
	bool has_sse3() { return ecx1[0]; }
	bool has_ssse3() { return ecx1[9]; }
	bool has_sse41() { return ecx1[19]; }
	bool has_sse42() { return ecx1[20]; }
	bool has_avx() { return ecx1[28]; }
	bool has_f16c() { return ecx1[29]; }
	bool has_avx2() { return ebx7[5]; }
	bool has_avx512_f() { return ebx7[16]; }
	bool has_avx512_dq() { return ebx7[17]; }
	bool has_avx512_ifma() { return ebx7[21]; }
	bool has_avx512_pf() { return ebx7[26]; }
	bool has_avx512_er() { return ebx7[27]; }
	bool has_avx512_cd() { return ebx7[28]; }
	bool has_sha() { return ebx7[29]; }
	bool has_avx512_bw() { return ebx7[30]; }
	bool has_avx512_vl() { return ebx7[31]; }
	bool has_avx512_vbmi() { return ecx7[1]; }
	bool has_avx512_vbmi2() { return ecx7[6]; }
	bool has_avx512_gfni() { return ecx7[8]; }
	bool has_avx512_vaes() { return ecx7[9]; }
	
	bool has_avx512_vpclmulqdq() { return ecx7[10]; }
	bool has_avx512_vnni() { return ecx7[11]; }
	bool has_avx512_bitalg() { return ecx7[12]; }
	bool has_avx512_vpopcntdq() { return ecx7[14]; }
	bool has_avx512_4vnniw() { return edx7[2]; }
	bool has_avx512_4fmaps() { return edx7[3]; }
	bool has_avx512_vp2intersect() { return edx7[8]; }
	bool has_avx512_bf16() { return eax7_1[5]; }
	bool has_avx512_fp16() { return edx7[23]; }


	//Returns a multiline string to show user their supported features.
	std::string to_string(){
		std::string s{};
		s += "Has SSE                 : " + yes_no(has_sse()) + "\n";
		s += "Has SSE2                : " + yes_no(has_sse2()) + "\n";
		s += "Has SSE3                : " + yes_no(has_sse3()) + "\n";
		s += "Has SSE4.1              : " + yes_no(has_sse41()) + "\n";
		s += "Has SSE4.2              : " + yes_no(has_sse42()) + "\n";
		s += "Has AVX                 : " + yes_no(has_avx()) + "\n";
		s += "Has AVX2                : " + yes_no(has_avx2()) + "\n";
		s += "Has AVX512 F            : " + yes_no(has_avx512_f()) + "\n";
		s += "Has AVX512 CD           : " + yes_no(has_avx512_cd()) + "\n";
		//s += "Has AVX512 ER           : " + yes_no(has_avx512_er()) + "\n";
		//s += "Has AVX512 PF           : " + yes_no(has_avx512_pf()) + "\n";
		//s += "Has AVX512 4FMAPS       : " + yes_no(has_avx512_4fmaps()) + "\n";
		//s += "Has AVX512 4VNNIW       : " + yes_no(has_avx512_4vnniw()) + "\n";
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