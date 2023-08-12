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

    The host independant renderer for the project.

*******************************************************************************************************/
#pragma once

#include <concepts>
#include <string>
#include <vector>
#include <numbers>
#include <typeinfo>

#include "../../common/colour.h"
#include "../../common/linear-algebra.h"
#include "../../common/noise.h"
#include "../../common/parameter-list.h"
#include "..\..\common\input-transforms.h"

#include "..\..\common\simd-cpuid.h"
#include "..\..\common\simd-f32.h"
#include "..\..\common\simd-uint32.h"
#include "..\..\common\simd-concepts.h"



/**************************************************************************************************
 * The renderer class.
 * Implements a host independent pixel renderer.
 * Use type parameter to select floating point precision.
 * ************************************************************************************************/
template <SimdFloat S>
class Renderer{
    

    private:
        int width {};
        int height {};
        S::F width_f {};
        S::F height_f {};
        S::F aspect {};
        std::string seed_string{};
        uint32_t seed{};
        ParameterList params{};

    public:
        //Constructor
        Renderer() noexcept {}

        //Size
        void set_size(int width, int height) noexcept;
        int get_width() const  { return width;}
        int get_height() const { return height;}

        //Set the seed as a string (an integer seed will be calculated)
        void set_seed(const std::string & s){
            this->seed=string_to_seed(s);             
            this->seed_string = s; 
        }
        //Set an integer seed. (string will be ignored)
        void set_seed_int(uint32_t s){
            this->seed = s;
        }
        std::string get_seed() const { return seed_string;}
        uint32_t get_seed_int() const { return seed;}
        
        //Parameters
        void set_parameters(ParameterList plist){
            params = plist;
        }

        //Render
        ColourRGBA<S> render_pixel(S x, S y) const;
        ColourRGBA<S> render_pixel_with_input(S x, S y, ColourRGBA<S>) const;

    private:


};


/**************************************************************************************************
 * Set the size of the image to render in pixels.
 * ************************************************************************************************/
template <SimdFloat S>
void Renderer<S>::set_size(int w, int h) noexcept {
    this->width = w;
    this->height = h;
    this->width_f = static_cast<S::F>(w);
    this->height_f = static_cast<S::F>(h);
    if (height==0) return;
    this->aspect = width_f/height_f;
}





/**************************************************************************************************
 * Render a pixel (or batch of pixels if using SIMD)
 * 
 * ************************************************************************************************/
template <SimdFloat S>
ColourRGBA<S> Renderer<S>::render_pixel(S x, S y) const {
    if (width <=0 || height <=0) return ColourRGBA<S>{};
    next_random<typename S::F>(seed); //Reset random seed so it is the same for each pixel
    
   
    

    
    return ColourRGBA{1.0f,0.0f,0.0f};  

}    

/**************************************************************************************************
 * Render a pixel (or batch of pixels if using SIMD)
 * an input pixel is given
 * ************************************************************************************************/
template <SimdFloat S>
ColourRGBA<S> Renderer<S>::render_pixel_with_input(S x, S y, ColourRGBA<S>) const {
    render_pixel(x, y);
}

















