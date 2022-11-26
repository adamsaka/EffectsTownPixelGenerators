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

    The host independant renderer for the project.

*******************************************************************************************************/
#pragma once

#include <concepts>
#include <string>
#include <vector>
#include <numbers>

#include "../../common/colour.h"
#include "../../common/linear-algebra.h"
#include "../../common/noise.h"
#include "../../common/parameter-list.h"



/**************************************************************************************************
 * The renderer class.
 * Implements a host independent pixel renderer.
 * Use type parameter to select floating point precision.
 * ************************************************************************************************/
template <std::floating_point F>
class Renderer{
    private:
        int width {};
        int height {};
        F width_f {};
        F height_f {};
        F aspect {};
        std::string seed_string{};
        uint64_t seed{};
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
        void set_seed_int(uint64_t s){
            this->seed = s;
        }
        std::string get_seed() const { return seed_string;}
        uint64_t get_seed_int() const { return seed;}
        
        //Parameters
        void set_parameters(ParameterList plist){
            params = plist;
        }

        //Render
        ColourSRGB<F> render_pixel(int x, int y) const;

    private:


};


/**************************************************************************************************
 * Set the size of the image to render in pixels.
 * ************************************************************************************************/
template <std::floating_point F>
void Renderer<F>::set_size(int w, int h) noexcept {
    this->width = w;
    this->height = h;
    this->width_f = static_cast<F>(w);
    this->height_f = static_cast<F>(h);
    if (height==0) return;
    this->aspect = width_f/height_f;
}





/**************************************************************************************************
 * 
 * ************************************************************************************************/
template <std::floating_point F>
ColourSRGB<F> Renderer<F>::render_pixel(int x, int y) const {
    if (width <=0 || height <=0) return ColourSRGB<F>{};
    next_random<F>(seed); //Reset random seed so it is the same for each pixel
    
    F parameter_scale = static_cast<F>(params.get_value(ParameterID::scale));
    F parameter_directional_bias = static_cast<F>(params.get_value(ParameterID::directional_bias));
    
    if (parameter_scale <= 0.0) parameter_scale = static_cast<F>(0.000001);

    auto xf = static_cast<F>(x) ;
    auto yf = static_cast<F>(y) ;
    


    //Normalise to range: Hight = -1..1  Width = proportional zero centered.
    vec2<F> p(aspect* (static_cast<F>(2.0)*xf/width_f- static_cast<F>(1.0)) , static_cast<F>(2.0) * yf/height_f - static_cast<F>(1.0));
 
    
    //auto r1 = next_random<F>();
    vec2<F> d{ 1.0,1.0 };
    if (signbit(parameter_directional_bias)) d.x -= parameter_directional_bias; else d.y += parameter_directional_bias;
    
    p = p* normalize(d) * static_cast<F>(sqrt(2)) * parameter_scale;
    
    auto nVec2 = p +     vec2(fbm(p,10,seed),fbm(p+ static_cast<F>(1.0),10,seed))- static_cast<F>(0.5);
    auto nVec3 = nVec2 + vec2(fbm(nVec2 + static_cast<F>(5.0),  8, seed), fbm(nVec2+ static_cast<F>(9.0),  8, seed))- static_cast<F>(0.5);
    auto nVec4 = nVec3 + vec2(fbm(nVec3 + static_cast<F>(25.0), 8, seed), fbm(nVec3+ static_cast<F>(19.0), 8, seed))- static_cast<F>(0.5);
    auto nVec5 = nVec4 + vec2(fbm(nVec4 - static_cast<F>(2.0),  5, seed), fbm(nVec4- static_cast<F>(19.0), 5, seed))- static_cast<F>(0.5);
    auto nVec6 = nVec5 + vec2(fbm(nVec5 - static_cast<F>(5.0),  5, seed), fbm(nVec5- static_cast<F>(9.0),  5, seed))- static_cast<F>(0.5);
    auto nVec7 = nVec6 + vec2(fbm(nVec6 - static_cast<F>(8.0),  5, seed), fbm(nVec6- static_cast<F>(1.0),  5, seed))- static_cast<F>(0.5);
    
    auto r = fbm(nVec5,8,seed)*0.68;
    auto g = fbm(nVec6,8,seed)*0.68;
    auto b = fbm(nVec7,8,seed)*0.68;
	return ColourSRGB{r,g,b};
    





    //auto a = fract(p*2.0);
    
    //return ColourSRGB<F>(a.x,a.y,1.0);
}    















