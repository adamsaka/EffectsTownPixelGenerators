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

    Render functions for After Effects host.

*******************************************************************************************************/
#pragma once

#pragma once
#include "after-effects-sdk.h"

void after_effects_smart_pre_render(const PF_InData* in_data, PF_PreRenderExtra* preRender);
void after_effects_smart_render(PF_InData* in_data, PF_OutData* out_data, PF_SmartRenderExtra* smartRender);
void after_effects_non_smart_render(PF_InData* in_data, PF_OutData* out_data, PF_ParamDef* params[], PF_LayerDef* output);