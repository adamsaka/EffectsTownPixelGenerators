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

	A list of parameterID to refer to each parameter.

	After Effects requires that ID remain the same accross different versions.
	So, do not remove ununsed parameters from list, just add new ones.


*******************************************************************************************************/
#pragma once

enum class ParameterID {
	input = 0,	//Reserve ID zero (for AE).
	seed,

	__last  //Must be last (used for array memory allocation)
};


constexpr int parameter_id_to_int(ParameterID p) noexcept { return static_cast<int>(p); }