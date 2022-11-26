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

//Project Specific includes
#include "config.h"
#include "renderer.h"
#include "parameters.h"

//General Includes
#include "after-effects-render.h"
#include "after-effect-parameter-helper.h"
#include "..\..\common\util.h"


/*******************************************************************************************************
Converts a colour component from the renderer to an 8-bit component
*******************************************************************************************************/
inline static uint8_t to_adobe_8bit(Precision c) { 
	constexpr unsigned short adobe_white8 = 0xff;
	return static_cast<uint8_t>(clamp(c * white8, static_cast<Precision>(0.0), static_cast<Precision>(adobe_white8)));
}

/*******************************************************************************************************
Converts a colour component from the renderer to an 16-bit component
Note: Adobe 16 bit is not full 16-bit.  White is 0x8000
*******************************************************************************************************/
inline static uint16_t to_adobe_16bit(Precision c) {
	constexpr unsigned short adobe_white16 = 0x8000;
	return static_cast<uint16_t>(clamp(c * adobe_white16, static_cast<Precision>(0.0), static_cast<Precision>(adobe_white16)));
}

/*******************************************************************************************************
Callback for After Effects Iteration Suite.  Renders an 8-bit pixel.
*******************************************************************************************************/
static PF_Err render_8bit_pixel_callback(void* refcon, A_long x, A_long y, [[maybe_unused]]  PF_Pixel8* in, PF_Pixel8* out) noexcept {
	const auto renderer = static_cast<Renderer<Precision>*>(refcon);
	const auto c = renderer->render_pixel(x, y);
	
	//Note: Adobe uses ARGB colour order, with unmultiplied alpha.
	out->alpha = to_adobe_8bit(c.alpha);
	out->red = to_adobe_8bit(c.red);
	out->green = to_adobe_8bit(c.green);
	out->blue = to_adobe_8bit(c.blue);
	return PF_Err_NONE;

}

/*******************************************************************************************************
Callback for After Effects Iteration Suite.  Renders an 16-bit pixel.
*******************************************************************************************************/
static PF_Err render_16bit_pixel_callback(void* refcon, A_long x, A_long y, [[maybe_unused]] PF_Pixel16* in, PF_Pixel16* out) noexcept {
	const auto renderer = static_cast<Renderer<Precision>*>(refcon);
	const auto c = renderer->render_pixel(x, y);

	//Note: Adobe uses ARGB colour order, with unmultiplied alpha.
	out->alpha = to_adobe_16bit(c.alpha);
	out->red = to_adobe_16bit(c.red);
	out->green = to_adobe_16bit(c.green);
	out->blue = to_adobe_16bit(c.blue);
	return PF_Err_NONE;
}

/*******************************************************************************************************
Callback for After Effects Iteration Suite.  Renders an 32-bit pixel.
*******************************************************************************************************/
static PF_Err render_32bit_pixel_callback(void* refcon, A_long x, A_long y, [[maybe_unused]] PF_Pixel32* in, PF_Pixel32* out) noexcept {
	const auto renderer = static_cast<Renderer<Precision>*>(refcon);
	const auto c = renderer->render_pixel(x, y);

	//Note: Adobe uses ARGB colour order, with unmultiplied alpha.
	out->alpha = static_cast<float>(c.alpha);
	out->red = static_cast<float>(c.red);
	out->green = static_cast<float>(c.green);
	out->blue = static_cast<float>(c.blue);
	return PF_Err_NONE;

}


/*******************************************************************************************************
Calculates the width and height of the image we are working with.  (From layer size)
Size may vary in low resolution renders.
*******************************************************************************************************/
static std::tuple<int, int> calculate_size(const PF_InData* in_data) {
	//We may be redering a low resolution version, get the scaleFactor
	const float scaleFactorX = static_cast<float>(in_data->downsample_x.den) / static_cast<float>(in_data->downsample_x.num);
	const float scaleFactorY = static_cast<float>(in_data->downsample_y.den) / static_cast<float>(in_data->downsample_y.num);

	//Calculate the size of our output using the full layer size.
	int width = static_cast<int>(static_cast<float>(in_data->width) / scaleFactorX);
	int height = static_cast<int>(static_cast<float>(in_data->height) / scaleFactorY);

	return std::tuple<int, int>(width, height);
}

/*******************************************************************************************************
Read Parameters from After Effects and put into data into our parameter list.
*******************************************************************************************************/
ParameterList read_parameters() {
	auto params = build_project_parameters();
	for (auto & p : params.entries) {
		switch (p.type) {
		case ParameterType::seed:
			p.value = ParameterHelper::ReadSlider(p.id);
			break;
		case ParameterType::number:
			p.value = ParameterHelper::ReadSlider(p.id);			
			break;

		default:
			break;
		}
	}

	return params;
}


/*******************************************************************************************************
Setup Host Independant Renderer
*******************************************************************************************************/
void setup_render(Renderer<Precision>& renderer, const PF_InData* in_data, int width, int height) {
	check_null(in_data);
	auto params = read_parameters();
	
	renderer.set_size(width, height);
	renderer.set_seed("After Effects");
	if (params.contains(ParameterID::seed)) {
		renderer.set_seed_int(static_cast<uint64_t>(params.get_value(ParameterID::seed)));
	}
	
	renderer.set_parameters(std::move(params));

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

	//Setup Host Independant Renderer
	Renderer<Precision> renderer{};
	setup_render(renderer, in_data, width, height);

	//Checkout the input buffer
	PF_EffectWorld* inputLayer{ nullptr };
	check_after_effects(smartRender->cb->checkout_layer_pixels(in_data->effect_ref, 0, &inputLayer));
	if (!inputLayer) throw (std::exception("Unable to checkout input layer."));

	//Checkout Output buffer
	PF_EffectWorld* output{ nullptr };
	check_after_effects(smartRender->cb->checkout_output(in_data->effect_ref, &output));
	if (!output) throw (std::exception("Unable to checkout input layer."));




	PF_Rect area;
	area.left = 0;
	area.right = output->width;
	area.top = 0;
	area.bottom = output->height;
	
	AEGP_SuiteHandler suites(in_data->pica_basicP);

	switch (smartRender->input->bitdepth) {
	case 8:
	{
		check_after_effects(suites.Iterate8Suite1()->iterate(in_data, 0, output->height, inputLayer, &area,  &renderer, render_8bit_pixel_callback, output));
		break;
	}
	case 16: {
		check_after_effects(suites.Iterate16Suite1()->iterate(in_data, 0, output->height, inputLayer, &area, &renderer, render_16bit_pixel_callback, output));
		break;
	}
	case 32: {
		check_after_effects(suites.IterateFloatSuite1()->iterate(in_data, 0, output->height, inputLayer, &area, &renderer, render_32bit_pixel_callback, output));
		break;
	}
	default: break;
	}
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
	
	//Setup Host Independant Renderer
	Renderer<Precision> renderer{};
	setup_render(renderer, in_data, width, height);


	PF_Rect area;
	area.left = 0;
	area.right = width;
	area.top = 0;
	area.bottom = height;

	auto inputLayer = &params[0]->u.ld;
	AEGP_SuiteHandler suites(in_data->pica_basicP);
	if (output->world_flags & PF_WorldFlag_DEEP) {
		//16-bit
		check_after_effects(suites.Iterate16Suite1()->iterate(in_data, 0, output->height, inputLayer, &area, &renderer, render_16bit_pixel_callback, output));
	}
	else {
		//8-bit
		check_after_effects(suites.Iterate8Suite1()->iterate(in_data, 0, output->height, inputLayer, &area, &renderer, render_8bit_pixel_callback, output));
	}

}