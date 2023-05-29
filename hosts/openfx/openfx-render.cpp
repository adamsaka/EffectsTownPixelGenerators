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

	Render functions for the openFX host.

********************************************************************************************************/
#pragma once
#include "openfx-render.h"
#include "openfx-parameter-helper.h"
#include "openfx-instance-data.h"

//Project Specific Includes
#include "renderer.h"
#include "parameters.h"
#include "config.h"
#include "..\..\common\linear-algebra.h"

#include "..\..\common\simd-cpuid.h"
#include "..\..\common\simd-f32.h"
#include "..\..\common\simd-uint32.h"


#include <bit>

static void ReplaceTransparentWithSource(OfxRectI renderWindow, ClipHolder& source, ClipHolder& output) noexcept;

static ParameterList read_parameters(ParameterHelper& parameter_helper, OfxTime time);

template <typename S>
struct RenderThreadData {
    Renderer<S>* renderer;
    ClipHolder* output;
    OfxRectI* render_window;
};

template <typename S>
static void render_line(RenderThreadData<S>* rd, int y) {
    //dev_log("Render Line " + std::to_string(y));
    
    int x = rd->render_window->x1;
    for (; x < rd->render_window->x2 - S::number_of_elements() + 1; x += S::number_of_elements() ) {
        const auto c = rd->renderer->render_pixel(S::make_sequential(static_cast<S::F>(x)), S(static_cast<S::F>(y)));
        copy_pixel_to_output_buffer(*rd->output, x, y, rd->render_window->x2, c);
    }
    //Handle the case where the width is not a multiple of S::number_of_elements
    if (x < rd->render_window->x2 && rd->render_window->x2 > S::number_of_elements()) [[unlikely]] {
        x -= S::number_of_elements() - (rd->render_window->x2 - x);
        const auto c = rd->renderer->render_pixel(S::make_sequential(static_cast<S::F>(x)), S(static_cast<S::F>(y)));
        copy_pixel_to_output_buffer(*rd->output, x, y, rd->render_window->x2, c);

    }
}


/*******************************************************************************************************

*******************************************************************************************************/
template <typename S>
void thread_entry_pixel_render(unsigned int threadIndex, [[maybe_unused]] unsigned int threadMax, void* customArg) {
    RenderThreadData<S>* rd = static_cast<RenderThreadData<S>*>(customArg);

    for (int y = rd->render_window->y1; y < rd->render_window->y2; y++) {
        if (y % threadMax == threadIndex) {
            render_line(rd, y);
        }
    }
}


/*******************************************************************************************************

*******************************************************************************************************/
template <typename S>
static void do_pixel_render(OfxImageEffectHandle instance, OfxRectI& render_window, Renderer<S>& renderer, [[maybe_unused]] int width, [[maybe_unused]] int height, ClipHolder& output) {
    RenderThreadData<S> rd{};
    rd.renderer = &renderer;
    rd.output = &output;
    rd.render_window = &render_window;



    unsigned int num_threads;
    global_MultiThreadSuite->multiThreadNumCPUs(&num_threads);
    //dev_log(std::string("Number of threads : ") + std::to_string(num_threads));

    if (num_threads > 1) [[likely]] {
        global_MultiThreadSuite->multiThread(thread_entry_pixel_render<S>, num_threads, &rd);
    }
    else {
        for (int y = render_window.y1; y < render_window.y2; y++) {
            if (global_EffectSuite->abort(instance)) return;
            render_line(&rd, y);
        }
    }
}


/*******************************************************************************************************

*******************************************************************************************************/
template <typename S>
static void setup_render(Renderer<S>& renderer, int width, int height, ParameterHelper& parameter_helper, OfxTime time) {

    auto params = read_parameters(parameter_helper, time);

    renderer.set_size(width, height);
    renderer.set_seed("OpenFX");
    if (params.contains(ParameterID::seed)) {
        renderer.set_seed_int(static_cast<uint64_t>(std::bit_cast<uint32_t>(params.get_value_integer(ParameterID::seed))));
    }


    renderer.set_parameters(std::move(params));
}

/*******************************************************************************************************
Render
*******************************************************************************************************/
OfxStatus openfx_render(const OfxImageEffectHandle instance, OfxPropertySetHandle in_args) {
    //dev_log(std::string("Render Action"));
    
    //Get the instance data
    InstanceData* instance_data{ nullptr };
    OfxPropertySetHandle effectProps;
    global_EffectSuite->getPropertySet(instance, &effectProps);
    global_PropertySuite->propGetPointer(effectProps, kOfxPropInstanceData, 0, (void**)&instance_data);

    
    //Get the context;
    char* cstr;
    OfxPropertySetHandle instanceProperties;
    check_openfx(global_EffectSuite->getPropertySet(instance, &instanceProperties));
    global_PropertySuite->propGetString(instanceProperties, kOfxImageEffectPropContext, 0, &cstr);
    //dev_log(std::string("Render Action - Context is: ") + cstr);
    [[maybe_unused]] auto context = GetContextFromString(cstr);

    //Get the current project time (we need it to checkout clips)
    OfxTime time{};
    check_openfx(global_PropertySuite->propGetDouble(in_args, kOfxPropTime, 0, &time));

    //Get the render window rectangle.
    OfxRectI renderWindow;
    check_openfx(global_PropertySuite->propGetIntN(in_args, kOfxImageEffectPropRenderWindow, 4, &renderWindow.x1));

    //Get the output clip handle 
    ClipHolder output_clip(instance, "Output", time);

    //Get Dimensions
    const int width = output_clip.bounds.x2 - output_clip.bounds.x1;
    const int height = output_clip.bounds.y2 - output_clip.bounds.y1;
    //dev_log(std::string("Size: " + std::to_string(width) + " x " + std::to_string(height)));


    //Runtime CPU Dispatch
    CpuInformation cpu_info{};
    if (Simd512UInt64::cpu_supported(cpu_info) && Simd512Float32::cpu_supported(cpu_info) && Simd512UInt32::cpu_supported(cpu_info) ) {
        //AVX-512 & AVX-512DQ 
        Renderer<Simd512Float32> renderer{};
        setup_render(renderer, width, height, instance_data->parameter_helper, time);
        do_pixel_render(instance, renderWindow, renderer, width, height, output_clip);

    }else if (Simd256UInt64::cpu_supported(cpu_info) && Simd256Float32::cpu_supported(cpu_info) && Simd256UInt32::cpu_supported(cpu_info)){
        //AVX & AVX2
        Renderer<Simd256Float32> renderer{};       
        setup_render(renderer, width, height, instance_data->parameter_helper, time);       
        do_pixel_render(instance, renderWindow, renderer, width, height, output_clip);
    }
    else {
        //Fallback
        Renderer<FallbackFloat32> renderer{};
        setup_render(renderer, width, height, instance_data->parameter_helper, time);
        do_pixel_render(instance, renderWindow, renderer, width, height, output_clip);
    }





    //Get & Mix Souce image.
    if constexpr (project_uses_input) {
        if (context == OFXContext::general || context == OFXContext::filter) {
            //There should be an input image. 
            ClipHolder inputClip(instance, "Source", time);
            ReplaceTransparentWithSource(renderWindow, inputClip, output_clip);
        }
    }

    if constexpr (!project_is_solid_render) {
        if (output_clip.preMultiplied && output_clip.componentsPerPixel == 4) {
            //TODO
            dev_log("***TODO: Multiply Alpha");
        }
    }

    return kOfxStatOK;
}



/*******************************************************************************************************

*******************************************************************************************************/
static ParameterList read_parameters(ParameterHelper& parameter_helper, OfxTime time) {
    auto params = build_project_parameters();
    
    for (auto& p : params.entries) {
        switch (p.type) {
        case ParameterType::seed:
            p.value_integer = parameter_helper.read_integer(p.id, time);
            break;
        case ParameterType::number:
            p.value = parameter_helper.read_slider(p.id, time);
            break;
        case ParameterType::list:
        {
            int i = parameter_helper.read_list(p.id, time);
            p.value_string = p.list[i];
            break;
        }

        default:
            break;
        }
    }
    return params;
}

/*******************************************************************************************************

*******************************************************************************************************/
template <typename S>
inline static void copy_pixel_to_output_buffer(ClipHolder& output, int x, int y, int max_x, ColourSRGB<S> c) {
    const bool hasAlpha = output.componentsPerPixel == 4;

    if constexpr (!project_is_solid_render) {
        if (output.preMultiplied) c = c.premultiply_alpha();
    }

   
    switch (output.bitDepth) {
    case 8:
         {
            /*
            const auto ptrDest = output.pixelAddress8(x, y);
            if (ptrDest) {
                ptrDest[0] = static_cast<uint8_t>(clamp(c.red* static_cast<Precision>(white8), static_cast<Precision>(0.0),static_cast<Precision>(white8))); //red
                ptrDest[1] = static_cast<uint8_t>(clamp(c.green * static_cast<Precision>(white8), static_cast<Precision>(0.0), static_cast<Precision>(white8))); //green
                ptrDest[2] = static_cast<uint8_t>(clamp(c.blue * static_cast<Precision>(white8), static_cast<Precision>(0.0), static_cast<Precision>(white8))); //blue            
                
                if (hasAlpha) ptrDest[3] = static_cast<uint8_t>(clamp(c.blue * static_cast<Precision>(white8), static_cast<Precision>(0.0), static_cast<Precision>(white8))); //alpha
            }
            break;
            */
         }
    case 32:
        {
            
            
            for (int i = 0; i < S::number_of_elements(); i++) {
                if (x + i >= max_x) break;
                const auto ptrDest = output.pixelAddressFloat(x+i, y);
                ptrDest[0] = static_cast<float>(c.red.element(i)); 
                ptrDest[1] = static_cast<float>(c.green.element(i)); 
                ptrDest[2] = static_cast<float>(c.blue.element(i)); 
                if (hasAlpha) ptrDest[3] = static_cast<float>(c.alpha.element(i)); 

            }
            break;
        }
    default:
        dev_log("Unexpected Pixel Format");
    }
    
}












/*******************************************************************************************************
Assume the output already contains the top image.  Any transparent parts are filled with source.
*******************************************************************************************************/
[[maybe_unused]]
static void ReplaceTransparentWithSource(OfxRectI renderWindow, ClipHolder& source, ClipHolder& output) noexcept {
    if (output.componentsPerPixel != 4) return;

    if (output.bitDepth == 32) {
        for (int y = renderWindow.y1; y < renderWindow.y2; y++) {
            for (int x = renderWindow.x1; x < renderWindow.x2; x++) {
                const auto ptrTop = output.pixelAddressFloat(x, y);
                if (!ptrTop) continue;
                if (ptrTop[3] < 1.0f) {
                    const auto ptrBot = source.pixelAddressFloat(x, y);
                    if (!ptrBot) continue;
                    ptrTop[0] = ptrTop[0] + ptrBot[0] * (1 - ptrTop[3]); //red
                    ptrTop[1] = ptrTop[1] + ptrBot[1] * (1 - ptrTop[3]); //green
                    ptrTop[2] = ptrTop[2] + ptrBot[2] * (1 - ptrTop[3]); //blue
                    ptrTop[3] = ptrTop[3] + ptrBot[3] * (1 - ptrTop[3]); //alpha
                }

            }
        }
    }
    if (output.bitDepth == 8) {
        //TODO
        dev_log("***TODO: 8 but mixing");
    }
}


