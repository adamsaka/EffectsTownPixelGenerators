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

Description:

	This is the list of parameters that will actually be displayed to the user.  
	This function and list is host-independant.

	
	Each entry must have a unique parameter-id

	After Effects requires that ID remain the same accross different versions.
	So, do not remove ununsed parameter-ids, just add new ones.

	Add ParameterIF::seed if you'd like to expose the random seed selection to user.

*******************************************************************************************************/

#include "parameters.h"
#include "parameter-id.h" 
#include "..\..\common\input-transforms.h"

ParameterList build_project_parameters() {
	ParameterList params;
	
	std::vector<std::string> input_list{};
	input_list.push_back("ACES Approx (Narkowicz)");
	params.add_entry(ParameterEntry::make_list(ParameterID::filmic_mode, "Filmic Curve", std::move(input_list)));

	params.add_entry(ParameterEntry::make_number(ParameterID::mix_amount, "Apply %",0.0,200.0,100.0,0.0,100.0,2));

	std::vector<std::string> colourspace_list{};
	colourspace_list.push_back("sRGB");
	colourspace_list.push_back("REC.709");
	colourspace_list.push_back("Linear");
	params.add_entry(ParameterEntry::make_list(ParameterID::colourspace_in, "Input Colour Space", colourspace_list));
	params.add_entry(ParameterEntry::make_list(ParameterID::colourspace_out, "Output Colour Space", std::move(colourspace_list)));
	
	
	//[NOT USED]
	//Input Transforms (builds from common set used in multiple projects)  
	//build_input_transforms_parameter_list(params);

	return params;
}