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

//Project Specific Includes
#include "renderer.h"
#include "parameters.h"
#include "config.h"

static void ReplaceTransparentWithSource(OfxRectI renderWindow, ClipHolder& source, ClipHolder& output) noexcept;
static void setup_render(Renderer<Precision>& renderer, int width, int height);
static ParameterList read_parameters();
static void do_pixel_render(Renderer<Precision>& renderer, int width, int height, ClipHolder& output);

/*******************************************************************************************************
Render
*******************************************************************************************************/
OfxStatus openfx_render(const OfxImageEffectHandle instance, OfxPropertySetHandle in_args) {
    dev_log(std::string("Render Action"));
    //Get the context;
    char* cstr;
    OfxPropertySetHandle instanceProperties;
    check_openfx(global_EffectSuite->getPropertySet(instance, &instanceProperties));
    global_PropertySuite->propGetString(instanceProperties, kOfxImageEffectPropContext, 0, &cstr);
    dev_log(std::string("Render Action - Context is: ") + cstr);
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

    //Setup platform independant Renderer
    Renderer<Precision> renderer{};
    setup_render(renderer, width, height);

    //Perform the actual render
    do_pixel_render(renderer, width, height, output_clip);


    //Get & Mix Souce image.
    /*
    if (context == OFXContext::general || context == OFXContext::filter) {
        //There should be an input image. 
        ClipHolder inputClip(instance, "Source", time);
        ReplaceTransparentWithSource(renderWindow, inputClip, output_clip);
    }
    */

    if (output_clip.preMultiplied && output_clip.componentsPerPixel == 4) {
        //TODO
        dev_log("***TODO: Multiply Alpha");
    }

    return kOfxStatOK;
}

/*******************************************************************************************************

*******************************************************************************************************/
static void setup_render(Renderer<Precision>& renderer, int width, int height) {
    auto params = read_parameters();

    renderer.set_size(width, height);
    renderer.set_seed("OpenFX");
    renderer.set_parameters(std::move(params));
}

/*******************************************************************************************************

*******************************************************************************************************/
static ParameterList read_parameters() {
    auto params = build_project_parameters();

    return params;
}

/*******************************************************************************************************

*******************************************************************************************************/
inline static void copy_pixel_to_output_buffer(ClipHolder& output, int x, int y, const ColourSRGB<Precision>& c) {
    const bool hasAlpha = output.componentsPerPixel == 4;

    switch (output.bitDepth) {
    case 8:
        dev_log("**8 bit copy");
        break;
    case 32:
        {
            const auto ptrDest = output.pixelAddressFloat(x, y);
            if (ptrDest) {
                ptrDest[0] = static_cast<float>(c.red); //red
                ptrDest[1] = static_cast<float>(c.green); //green
                ptrDest[2] = static_cast<float>(c.blue); //blue            
                //TODO: (MANAGE PREMULTIPLIED ALPHA)
                if (hasAlpha) ptrDest[3] = static_cast<float>(c.alpha) / 255.0f; //alpha
            }
            break;
        }
    default:
        dev_log("Unexpected Pixel Format");
    }
}

/*******************************************************************************************************

*******************************************************************************************************/
static void do_pixel_render(Renderer<Precision>& renderer, int width, int height, ClipHolder& output) {
    //Single threaded for now.
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            const auto c = renderer.render_pixel(x, y);
            copy_pixel_to_output_buffer(output, x, y, c);
        }
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


