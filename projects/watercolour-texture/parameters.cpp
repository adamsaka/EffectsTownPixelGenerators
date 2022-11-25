/********************************************************************************************************

Authors:		(c) 2022 Maths Town

Licence:		The GNU General Public License Version 3

*********************************************************************************************************
This program is free software: you can redistribute it and/or modify it under the terms of the
GNU General Public License as published by the Free Software Foundation, either version 3 of the License,
or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program.
If not, see <https://www.gnu.org/licenses/>.
********************************************************************************************************

Description:

	This is the list of parameters that will actually be displayed to the user.  
	Each entry must have a unique parameter-id

	After Effects requires that ID remain the same accross different versions.
	So, do not remove ununsed parameter-ids, just add new ones.

	Add ParameterIF::seed if you'd like to expose the random seed selection to user.

*******************************************************************************************************/

#include "parameters.h"
#include "parameter-id.h" 

ParameterList build_project_parameters() {
	ParameterList params;
	params.add_entry(ParameterEntry::make_seed(ParameterID::seed, "Random Seed"));
	params.add_entry(ParameterEntry::make_number(ParameterID::scale, "Scale",0.0000001,10000.0,1.0,0.000001,100.0,2));
	params.add_entry(ParameterEntry::make_number(ParameterID::directional_bias, "Directional Bias", -10000, 10000.0, 0.0, -100.0, 100.0, 2));

	return params;
}