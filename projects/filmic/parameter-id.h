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

Description:

	A list of parameterID to refer to each parameter.

	After Effects requires that ID remain the same accross different versions.
	So, do not remove ununsed parameters from list, just add new ones.

	Actual specification for project parameters in parameters.cpp


*******************************************************************************************************/
#pragma once



enum class ParameterID {
	input = 0,	   //Reserve ID zero (for AE).
	seed,		   //Reserved for Random Seed.
	seed_button,   //Reserved
	seed_int,	   //Reserved
	
	//Input Transforms.  Should keep in enum so code compiles, order only needs to remain the same for this project.
	input_transform_group_start,
	input_transform_group_end,
	input_transform_type,
	input_transform_scale,
	input_transform_rotation,
	input_transform_translate_x,
	input_transform_translate_y,
	input_transform_special1,
	input_transform_special2,
	input_transform_special3,
	input_transform_special4,

	//For this project
	filmic_mode,
	mix_amount,
	
	colourspace_in,
	colourspace_out,
	exposure,
	gamma,




	//Must be last (used for array memory allocation)
	__last  
};

constexpr int parameter_id_to_int(ParameterID p) noexcept { return static_cast<int>(p); }


