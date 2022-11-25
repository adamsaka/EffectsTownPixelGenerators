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
#include "config.h"
#include "after-effects-render.h"
#include "..\..\common\util.h"


std::tuple<int, int> calculate_size(const PF_InData* in_data) {
	//We may be redering a low resolution version, get the scaleFactor
	const float scaleFactorX = static_cast<float>(in_data->downsample_x.den) / static_cast<float>(in_data->downsample_x.num);
	const float scaleFactorY = static_cast<float>(in_data->downsample_y.den) / static_cast<float>(in_data->downsample_y.num);

	//Calculate the size of our output using the full layer size.
	int width = static_cast<int>(static_cast<float>(in_data->width) / scaleFactorX);
	int height = static_cast<int>(static_cast<float>(in_data->height) / scaleFactorY);
}


/*******************************************************************************************************
After Effects SmartPreRender Command
Called by After Effects to get information about the render.  
We need to set the output rectangles and checkout any layers that we need.
*******************************************************************************************************/
void after_effects_smart_pre_render(const PF_InData* in_data, PF_PreRenderExtra* preRender) {
	check_null(in_data);
	check_null(preRender);
	check_null(in_data->effect_ref);
	check_null(preRender->output);
	check_null(preRender->input);

	const auto [width, height] = calculate_size(in_data);

	//Checkout the input layer
	PF_CheckoutResult inputLayer{};
	check_after_effects(preRender->cb->checkout_layer(in_data->effect_ref, 0, 0, &preRender->input->output_request, in_data->current_time, in_data->time_step, in_data->time_scale, &inputLayer));
	const auto r = preRender->input->output_request.rect;
	const auto in = inputLayer.result_rect;


	//Max output rectangle.
	preRender->output->max_result_rect.top = 0;
	preRender->output->max_result_rect.bottom = height;
	preRender->output->max_result_rect.left = 0;
	preRender->output->max_result_rect.right = width;
	
	//Output rectangle
	preRender->output->result_rect.top = std::max(0, r.top);
	preRender->output->result_rect.bottom = std::min(height, r.bottom);
	preRender->output->result_rect.left = std::max(0, r.left);
	preRender->output->result_rect.right = std::min(width, r.right);

	//Is output solid?
	preRender->output->solid = project_is_solid_render; //Project setting from config.h
}

/*******************************************************************************************************
After Effects Smart Render Command
This render likely comes from After Effects.
*******************************************************************************************************/
void after_effects_smart_render(PF_InData* in_data, PF_OutData* out_data, PF_SmartRenderExtra* smartRender) {
	check_null(in_data);
	check_null(out_data);
	check_null(smartRender);

	const auto [width, height] = calculate_size(in_data);
}

/*******************************************************************************************************
After Effects Render Command (Old fashioned Non-Smart Mode)
This render likely comes from Premiere Pro.
*******************************************************************************************************/
void after_effects_non_smart_render(PF_InData* in_data, PF_OutData* out_data, PF_ParamDef* params[], PF_LayerDef* output) {
	check_null(in_data);
	check_null(out_data);
	check_null(output);
	check_null(params);

	const auto [width, height] = calculate_size(in_data);

}