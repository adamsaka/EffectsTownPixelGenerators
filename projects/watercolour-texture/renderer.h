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

#include "../../common/colour.h"
#include "../../common/linear-algebra.h"
#include "../../common/noise.h"



template <std::floating_point FloatType>
class Renderer{
    private:
        int width {};
        int height {};
        FloatType width_f {};
        FloatType height_f {};
        FloatType aspect {};
        std::string seed_string{};
        uint64_t seed{};

    public:
        //Constructor
        Renderer() noexcept {}

        //Size
        void set_size(int width, int height) noexcept;
        int get_width() const  { return width;}
        int get_height() const { return height;}

        //Seed
        void set_seed(const std::string & s){
            this->seed=string_to_seed(s);             
            this->seed_string = s; 
        }
        std::string get_seed() const { return seed_string;}
        uint64_t get_seed_int() const { return seed;}
        

        //Render
        ColourSRGB<FloatType> render_pixel(int x, int y) const;
};


/**************************************************************************************************
 * Set the size of the image to render in pixels.
 * ************************************************************************************************/
template <std::floating_point FloatType>
void Renderer<FloatType>::set_size(int w, int h) noexcept {
    this->width = w;
    this->height = h;
    this->width_f = static_cast<double>(w);
    this->height_f = static_cast<double>(h);
    if (height==0) return;
    this->aspect = width_f/height_f;
}


/**************************************************************************************************
 * 
 * ************************************************************************************************/
template <std::floating_point FloatType>
ColourSRGB<FloatType> Renderer<FloatType>::render_pixel(int x, int y) const {
    if (width <=0 || height <=0) return ColourSRGB<FloatType>{};
    next_random<FloatType>(seed); //Reset random seed so it is the same for each pixel
    
    auto xf = static_cast<FloatType>(x);
    auto yf = static_cast<FloatType>(y);
    


    //Normalise to range: Hight = -1..1  Width = proportional zero centered.
    vec2<FloatType> p(aspect* (2.0*xf/width_f-1.0) , 2.0*yf/height_f - 1.0);
 
    
    auto r1 = next_random<FloatType>();
    auto r2 = next_random<FloatType>();
    
    p = p*vec2(r1*4.0+0.5, r2*3.0+0.5);
    
    auto nVec2 = p + vec2(fbm(p,10,seed),fbm(p+1.0,10,seed))-0.5;
    auto nVec3 = nVec2 + vec2(fbm(nVec2+5.0, 8,seed),fbm(nVec2+9.0, 8,seed))-0.5;
    auto nVec4 = nVec3 + vec2(fbm(nVec3+25.0, 8,seed),fbm(nVec3+19.0, 8,seed))-0.5;
    auto nVec5 = nVec4 + vec2(fbm(nVec4-2.0, 5,seed),fbm(nVec4-19.0, 5,seed))-0.5;
    auto nVec6 = nVec5 + vec2(fbm(nVec5-5.0, 5,seed),fbm(nVec5-9.0, 5,seed))-0.5;
    auto nVec7 = nVec6 + vec2(fbm(nVec6-8.0, 5,seed),fbm(nVec6-1.0, 5,seed))-0.5;
    
    auto r = fbm(nVec5,8,seed)*0.68;
    auto g = fbm(nVec6,8,seed)*0.68;
    auto b = fbm(nVec7,8,seed)*0.68;
	return ColourSRGB{r,g,b};
    





    //auto a = fract(p*2.0);
    
    //return ColourSRGB<FloatType>(a.x,a.y,1.0);
}    


















