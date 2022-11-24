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

    Colour Types and routines

Types:

   Colour8   = 8 bit per pixel (RGBA colour order).  (sRGB Colour Space)
   ColourSRGB<F> = Floating point colour (F is float or double)  (0.0 .. 1.0 range)
   ColourLinear<F> = Floating point colour (F is float or double)  (0.0 .. 1.0 range)


*******************************************************************************************************/
#pragma once
#include <concepts>
#include <cstdint>
#include <cmath>
#include <string>

/****Forward Declaration of Types***/
struct Colour8;
template <std::floating_point FloatType> struct ColourLinear;
template <std::floating_point FloatType> struct ColourSRGB;

/****Forward Declaration of Functions***/
template <std::floating_point F> static constexpr F srgb_to_linear(F c) noexcept;
template <std::floating_point F> static constexpr uint8_t float_to_8bit(F c) noexcept;
template <std::floating_point F> static constexpr uint32_t float_to_uint(F c) noexcept;
template <std::floating_point F> static constexpr ColourSRGB<F> HSLtoRGB(F alpha, F h, F s, F l) noexcept;


/***Constants****/
constexpr unsigned char black8 = 0;
constexpr unsigned char white8 = 0xff;
constexpr float black32 = 0.0;
constexpr float white32 = 1.0;




/**************************************************************************************************
Colour8 Type
Type:           Generic 8 bit per pixel 
Gamma:          Generic
Colour Order :  RGBA
*************************************************************************************************/
struct Colour8 {
    public:
        uint8_t red {0};
        uint8_t green {0};
        uint8_t blue {0};
        uint8_t alpha {0xff};

        /**************************************************************************************************
        Constructors
        *************************************************************************************************/
        Colour8() = default;

        //Construct from components
        Colour8(uint8_t r, uint8_t g, uint8_t b, uint8_t a) noexcept {
            alpha = a; red = r; green = g; blue = b;
        }

        /**************************************************************************************************
            Convert to string.
        *************************************************************************************************/
        std::string to_string() const {            
            return "Colour8{r=" + std::to_string(red) + ", g="+ std::to_string(green) + ", b="+ std::to_string(blue) + ", a="+ std::to_string(alpha) + "}";
        }
    

        /*********************************************************************************************************
         Returns an unsigned int.
        RGBA colour order.
        (Mainly useful for string display.  Actual memory layout will be endian dependant)
        *********************************************************************************************************/
        uint32_t to_uint32() const noexcept{        
            return (static_cast<uint32_t>(red) << 24) | (static_cast<uint32_t>(green) << 16) | (static_cast<uint32_t>(blue) << 8 ) | (static_cast<uint32_t>(alpha) );
        }

        /*********************************************************************************************************
        Returns an unsigned 32bit int with the same memory layout as this struct.
        (Useful for packing into image buffers, and should work for either endian)
        *********************************************************************************************************/
        uint32_t to_uint32_keep_memory_layout() const noexcept{        
            static_assert (sizeof(Colour8) == 4, "Colour8 struct should be 4 bytes.");
            return *reinterpret_cast<const uint32_t *>(this);
        }

};




/**************************************************************************************************
ColourSRGB Type
Type:           Floating point (single or double by template)
Gamma:          SRGB
Colour Order :  RGBA

*************************************************************************************************/
template <std::floating_point FloatType>
struct ColourSRGB {
    public:
        FloatType red{ 0.0 };
        FloatType green{ 0.0 };
        FloatType blue{ 0.0 };
        FloatType alpha{ 1.0 };

        /**************************************************************************************************
        Constructors
        *************************************************************************************************/

        //Default Contructor
        ColourSRGB() = default;

        ///Construct from components
        ColourSRGB(FloatType r, FloatType g, FloatType b, FloatType a = 1.0) noexcept {
            red = r; green = g; blue = b; alpha = a; 
        }

        ///Construct from 8-bit components
        ColourSRGB(uint8_t r, uint8_t g, uint8_t b, uint8_t a=0xff) noexcept {
            red = static_cast<FloatType>(r) / white8;
            green = static_cast<FloatType>(g) / white8;
            blue = static_cast<FloatType>(b) / white8;
            alpha = static_cast<FloatType>(a) / white8;
        }

        ///Construct from ColourLinear (float or double)
        template <std::floating_point F>
        ColourSRGB(const ColourSRGB<F> & c) noexcept {		
            red = static_cast<FloatType>(c.red);
            green = static_cast<FloatType>(c.green);
            blue = static_cast<FloatType>(c.blue);
            alpha = static_cast<FloatType>(c.alpha);
        }

        //Construct from HSL values.  All inputs 0..1
        static ColourSRGB<FloatType> from_hsl(FloatType alpha, FloatType hue, FloatType saturation, FloatType lightness){
            return HSLtoRGB<FloatType>(alpha, hue, saturation, lightness);
        }

        /**************************************************************************************************
        Convert to string.
        *************************************************************************************************/
        std::string to_string() const {
            return "ColourSRGB{r=" + std::to_string(red) + ", g="+ std::to_string(green) + ", b="+ std::to_string(blue) + ", a="+ std::to_string(alpha) + "}";
        }

        

        /**************************************************************************************************
        Convert to a linear colour.
        Assumes:  1. Not premultiplied alpha.
                  2. that no gamma is applied to the alpha channel.
        *************************************************************************************************/
        ColourLinear<FloatType> to_linear() const noexcept {
            return ColourLinear<FloatType>(srgb_to_linear(red), srgb_to_linear(green), srgb_to_linear(blue), alpha);
        }    

        /**************************************************************************************************
        Convert to simple using a simple 2.2 gamma adjustment (may be faster than piecewise method).
        Assumes:  1. Not premultiplied alpha.
                  2. that no gamma is applied to the alpha channel.
        *************************************************************************************************/
        ColourLinear<FloatType> to_linear_simple() const noexcept {
            return ColourLinear<FloatType>(std::pow(red, static_cast<FloatType>(2.2)), std::pow(green, static_cast<FloatType>(2.2)), std::pow(blue, static_cast<FloatType>(2.2)), alpha);
        } 

        /**************************************************************************************************
        Convert to Colour8
        *************************************************************************************************/
        Colour8 to_colour8() const noexcept {
            return Colour8(float_to_8bit(red), float_to_8bit(green), float_to_8bit(blue), float_to_8bit(alpha) );
        }

        /**************************************************************************************************
        Returns colour as an unsigned int.        
        RGBA colour order.        
        *************************************************************************************************/
        uint32_t to_uint32() const noexcept {          
            return  (float_to_uint(red) << 24) | (float_to_uint(green) << 16) | (float_to_uint(blue) << 8 ) | (float_to_uint(alpha) );            
        }



        /**************************************************************************************************
        Multiplies the alpha channel through the colour. (For pre-mulitplied alpha buffers)
        Note: Routines in this library assume alpha is not pre-multiplied.
        *************************************************************************************************/
        ColourSRGB<FloatType> premultiply_alpha() const noexcept {
            auto c = *this;
            c.red *= c.alpha;
            c.green *= c.alpha;
            c.blue *= c.alpha;
            return c;
        }

        /**************************************************************************************************
        Un-multiplies the alpha channel through the colour. (For pre-mulitplied alpha buffers)
        *************************************************************************************************/
        ColourSRGB<FloatType> un_premultiply_alpha() const noexcept {
            auto c = *this;
            c.red /= c.alpha;
            c.green /= c.alpha;
            c.blue /= c.alpha;
            return c;
        }

        /**************************************************************************************************
        Clamps all colour components to 0.0 ... 1.0
        *************************************************************************************************/
        ColourSRGB<FloatType> clamp() const noexcept {
            const auto r = (red < 0.0 ) ? 0.0 : ((red>1.0) ? 1.0 : red);
            const auto g = (green < 0.0 ) ? 0.0 : ((green>1.0) ? 1.0 : green);
            const auto b = (blue < 0.0 ) ? 0.0 : ((blue>1.0) ? 1.0 : blue);
            const auto a = (alpha < 0.0 ) ? 0.0 : ((alpha>1.0) ? 1.0 : alpha);
            return ColourSRGB<FloatType>(r,g,b,a);
        }  
           
};



/**************************************************************************************************
ColourLinear Type
Type:           Floating point (single or double by template)
Gamma:          Linear
Colour Order :  RGBA
*************************************************************************************************/
template <std::floating_point FloatType>
struct ColourLinear {
    public:
        FloatType red{ 0.0 };
        FloatType green{ 0.0 };
        FloatType blue{ 0.0 };
        FloatType alpha{ 1.0 };

        /**************************************************************************************************
        Constructors
        *************************************************************************************************/

        //Default Contructor
        ColourLinear() = default;

        ///Construct from components
        ColourLinear(FloatType r, FloatType g, FloatType b, FloatType a = 1.0) noexcept {
            red = r; green = g; blue = b; alpha = a; 
        }

        ///Construct from 8-bit components
        ColourLinear(uint8_t r, uint8_t g, uint8_t b, uint8_t a=0xff) noexcept {
            red = static_cast<FloatType>(r) / white8;
            green = static_cast<FloatType>(g) / white8;
            blue = static_cast<FloatType>(b) / white8;
            alpha = static_cast<FloatType>(a) / white8;
        }

        ///Construct from Colour8
        ColourLinear(const Colour8 & c) noexcept{
            red = static_cast<FloatType>(c.red) / white8;
            green = static_cast<FloatType>(c.green) / white8;
            blue = static_cast<FloatType>(c.blue) / white8;
            alpha = static_cast<FloatType>(c.alpha) / white8;
        }
        
        ///Construct from ColourLinear (float or double)
        template <std::floating_point F>
        ColourLinear(const ColourLinear<F> & c) noexcept {		
            red = static_cast<FloatType>(c.red);
            green = static_cast<FloatType>(c.green);
            blue = static_cast<FloatType>(c.blue);
            alpha = static_cast<FloatType>(c.alpha);
        }

        //construct from SRGB Colour
        template <std::floating_point F>
        ColourLinear(const ColourSRGB<F> & c) noexcept {		
            red = static_cast<FloatType>(c.red);
            green = static_cast<FloatType>(c.green);
            blue = static_cast<FloatType>(c.blue);
            alpha = static_cast<FloatType>(c.alpha);
        }

        /**************************************************************************************************
        Convert to string.
        *************************************************************************************************/
        std::string to_string() const {
            return "ColourLinear{r=" + std::to_string(red) + ", g="+ std::to_string(green) + ", b="+ std::to_string(blue) + ", a="+ std::to_string(alpha) + "}";
        }

        /**************************************************************************************************
        Convert to Colour8
        *************************************************************************************************/
        Colour8 to_colour8() const noexcept {
            return Colour8(float_to_8bit(red), float_to_8bit(green), float_to_8bit(blue), float_to_8bit(alpha) );
        }

        /**************************************************************************************************
        Returns colour as an unsigned int.        
        RGBA colour order.        
        *************************************************************************************************/
        uint32_t to_uint32() const noexcept {          
            return  (float_to_uint(red) << 24) | (float_to_uint(green) << 16) | (float_to_uint(blue) << 8 ) | (float_to_uint(alpha) );            
        }

    
        /**************************************************************************************************
        Clamps all colour components to 0.0 ... 1.0
        *************************************************************************************************/
        ColourLinear<FloatType> clamp() const noexcept {
            const auto r = (red < 0.0 ) ? 0.0 : ((red>1.0) ? 1.0 : red);
            const auto g = (green < 0.0 ) ? 0.0 : ((green>1.0) ? 1.0 : green);
            const auto b = (blue < 0.0 ) ? 0.0 : ((blue>1.0) ? 1.0 : blue);
            const auto a = (alpha < 0.0 ) ? 0.0 : ((alpha>1.0) ? 1.0 : alpha);
            return ColourLinear<FloatType>(r,g,b,a);
        } 

        /**************************************************************************************************
        Multiplies the alpha channel through the colour. (For pre-mulitplied alpha buffers)
        Note: Routines in this library assume alpha is not pre-multiplied.
        *************************************************************************************************/
        ColourLinear<FloatType> premultiply_alpha() const noexcept {
            auto c = *this;
            c.red *= c.alpha;
            c.green *= c.alpha;
            c.blue *= c.alpha;
            return c;
        }

        /**************************************************************************************************
        Un-multiplies the alpha channel through the colour. (For pre-mulitplied alpha buffers)
        *************************************************************************************************/
        ColourLinear<FloatType> un_premultiply_alpha() const noexcept {
            auto c = *this;
            c.red /= c.alpha;
            c.green /= c.alpha;
            c.blue /= c.alpha;
            return c;
        }

	

	//Do colour multiplication
	ColourLinear operator*(const ColourLinear& rhs) const noexcept{
		return ColourLinear{red * rhs.red, green*rhs.green, blue*rhs.blue, alpha * rhs.alpha };
	}


	ColourLinear operator*(FloatType rhs) const noexcept {
		return ColourLinear{red * rhs, green * rhs, blue * rhs, alpha * rhs};
	}



};



/**************************************************************************************************
Mix two colours together with a weight (0.0..1.0)
*************************************************************************************************/
template <std::floating_point F>
inline ColourLinear<F> mix_colours(const ColourLinear<F> & c1, const ColourLinear<F> & c2, F weight) noexcept {
	return ColourLinear<F>(
		c1.red * (1.0 - weight) + c2.red * weight,
		c1.green * (1.0 - weight) + c2.green * weight,
		c1.blue * (1.0 - weight) + c2.blue * weight,
        c1.alpha * (1.0-weight) + c2.alpha * weight
		);
}

template <std::floating_point F>
inline ColourSRGB<F> mix_colours(const ColourSRGB<F> & c1, const ColourSRGB<F> & c2, F weight) noexcept {
	return ColourSRGB<F>(
		c1.red * (1.0 - weight) + c2.red * weight,
		c1.green * (1.0 - weight) + c2.green * weight,
		c1.blue * (1.0 - weight) + c2.blue * weight,
        c1.alpha * (1.0-weight) + c2.alpha * weight
		);
}


/**************************************************************************************************
Converts a single component of sRGB colour to linear.
Uses the peicewise function as per the specification.  However, uses ^2.2 if outide 0.0 .. 1.0
*************************************************************************************************/
template <std::floating_point F>
static constexpr F srgb_to_linear(F c) noexcept{
    if (c > 0.04045 && c < 1.0) [[likely]] return std::pow((c+0.055)/1.055, static_cast<F>(2.4));      
    if (c <= 0.04045 && c > 0.0 ) return c/12.92;
    return std::pow(c, static_cast<F>(2.2));  //If outside range use 2.2
} 

/**************************************************************************************************
Converts a single colour component from float to 8-bit (returns as a 8 bit unsigned char)
*************************************************************************************************/
template <std::floating_point F> 
static constexpr uint8_t float_to_8bit(F c) noexcept {
    const F a = c * white8;
    if (a <= 0.0) return 0;
    if (a >= white8) return white8;
    return static_cast<uint8_t>(a);
}


/**************************************************************************************************
Converts a single colour component from float to 8-bit (returns as a 32 bit unsigned int)
*************************************************************************************************/
template <std::floating_point F> 
static constexpr uint32_t float_to_uint(F c) noexcept {
    const F a = c * white8;
    if (a <= 0.0) return 0;
    if (a >= white8) return white8;
    return static_cast<uint32_t>(a);
}




//Just a helper for HSL to RGB
template <std::floating_point F>
static constexpr  F _HuetoRGB(F v1, F v2, F h) noexcept {
	if (h < 0) h += 1;
	if (h > 1) h -= 1;
	if (h * 6 < 1) return v1 + (v2 - v1) * 6 * h;
	if (h * 2 < 1) return v2;
	if (h * 3 < 2) return v1 + (v2 - v1) * ((2.0 / 3.0) - h) * 6;
	return v1;
}

/**************************************************************************************************
Conver HSL to RGB.  HSL values are all 0..1
***************************************************************************************************/
template <std::floating_point F>
static constexpr ColourSRGB<F> HSLtoRGB(F alpha, F h, F s, F l) noexcept{
	if (s == 0) return Colour(alpha, l, l, l);
	const F v2 = (l< 0.5) ? l * (1 + s) : (l + s) - (l*s);
	const F v1 = 2 * l - v2;
	return Colour(alpha,_HuetoRGB<F>(v1,v2,h+(1.0/3.0)), _HuetoRGB<F>(v1, v2, h), _HuetoRGB<F>(v1, v2, h - (1.0 / 3.0)));
}




