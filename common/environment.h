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
* 
* Sets up environmental variables
* 
* 
*********************************************************************************************************/
#pragma once

namespace mt::environment {




#if defined(_M_X64) || defined(__x86_64)
	constexpr bool is_x64 = true;
#else
	constexpr bool is_x64 = false;
#endif

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


}


 


