/*********************************************************************************************************
This program is free software: you can redistribute it and/or modify it under the terms of the 
GNU General Public License as published by the Free Software Foundation, either version 3 of the License, 
or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program. 
If not, see <https://www.gnu.org/licenses/>. 

Authors:
	Adam - Maths Town
    Inigo Quilez (This file includes code snippets from his articles as documented in function comments)

*********************************************************************************************************/
#pragma once

#include "linear-algebra.h"
#include <concepts>
#include <cmath>
#include <limits>
#include <bit>




/**************************************************************************************************
 * "SplitMix64" random nuber generator 
 * (Public Domain)
 * https://rosettacode.org/wiki/Pseudo-random_numbers/Splitmix64
 * ************************************************************************************************/
static inline constexpr uint64_t split_mix_64(uint64_t state) {
	uint64_t z = (state += 0x9e3779b97f4a7c15);
	z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9;
	z = (z ^ (z >> 27)) * 0x94d049bb133111eb;
	return z ^ (z >> 31);
}

/**************************************************************************************************
 * Gets a random float,
 * Use state variable on first call only to set seed.
 * ************************************************************************************************/
template <std::floating_point F>
static F next_random(uint64_t state = 0) {
    static uint64_t random_state;
    if (state != 0) random_state = state;
    random_state = split_mix_64(random_state);
    return static_cast<F>(random_state) / static_cast<F>(std::numeric_limits<uint64_t>::max());
} 

/**************************************************************************************************
 * Bitcasting of floats to ints
 * ************************************************************************************************/
static inline constexpr uint64_t cast_to_int64(float f){
    return static_cast<uint64_t>(std::bit_cast<uint32_t>(f));
}
static inline constexpr uint64_t cast_to_int64(double f){
    return std::bit_cast<uint64_t>(f);
}

/**************************************************************************************************
 * Hash based on coordinates and a seed value
 * ************************************************************************************************/
template <std::floating_point F>
static inline constexpr  F hash(F coordinate, uint64_t seed = 0){
    uint64_t seed64 = cast_to_int64(coordinate);   
    seed64 ^= seed;
    return static_cast<F>(split_mix_64(seed64)) / static_cast<F>(std::numeric_limits<uint64_t>::max());
}

template <std::floating_point F>
static inline constexpr  F hash(vec2<F> coordinate, uint64_t seed = 0){
    uint64_t seed64 = seed; 
    seed64 ^= cast_to_int64(coordinate.x);   
    seed64 ^= std::rotr(cast_to_int64(coordinate.y),32);    
    return static_cast<F>(split_mix_64(seed64)) / static_cast<F>(std::numeric_limits<uint64_t>::max());
}

template <std::floating_point F>
static inline constexpr  F hash(vec3<F> coordinate, uint64_t seed = 0){
    uint64_t seed64 = seed; 
    seed64 ^= cast_to_int64(coordinate.x);   
    seed64 ^= std::rotr(cast_to_int64(coordinate.y),12);
    seed64 ^= std::rotr(cast_to_int64(coordinate.z),24);    
    return static_cast<F>(split_mix_64(seed64)) / static_cast<F>(std::numeric_limits<uint64_t>::max());
}

template <std::floating_point F>
static inline constexpr F hash(vec4<F> coordinate, uint64_t seed = 0){
    uint64_t seed64 = seed; 
    seed64 ^= cast_to_int64(coordinate.x);   
    seed64 ^= std::rotr(cast_to_int64(coordinate.y),8);
    seed64 ^= std::rotr(cast_to_int64(coordinate.z),16);
    seed64 ^= std::rotr(cast_to_int64(coordinate.w),24);    
    return static_cast<F>(split_mix_64(seed64)) / static_cast<F>(std::numeric_limits<uint64_t>::max());
}

/**************************************************************************************************
 * Simple method to convert a string to a uint64 seed
 * ************************************************************************************************/
static inline constexpr uint64_t string_to_seed(const std::string& str){
    uint64_t seed {0};
    for (size_t i=0;i<str.length();i++){
        seed ^= static_cast<uint64_t>(str[i]) >> ((i & 8)*8); 
    }
    return seed;
}




/**************************************************************************************************
Value Noise
*************************************************************************************************/
template <std::floating_point F>
static F value_noise(vec2<F> p , uint64_t seed=0){
    auto i = floor(p);
    auto f = fract_negative_adjust(p);  //Adjusting the negative values of fract makes smooth noise for negative inputs
    auto u = f*f*(3.0-2.0*f);
    return mix( mix( hash( i + vec2(0.0,0.0), seed ), hash( i + vec2(1.0,0.0),seed ), u.x),mix( hash( i + vec2(0.0,1.0),seed ),hash( i + vec2(1.0,1.0),seed ), u.x), u.y);
}


/**************************************************************************************************
Based on article by Inigo Quilez https://www.iquilezles.org/www/articles/fbm/fbm.htm
The original code snippet was released under the MIT license: https://opensource.org/licenses/MIT
*************************************************************************************************/
template <std::floating_point F>
static F fbm(vec2<F> x, int number_octaves = 8, uint64_t seed=0){    
    const F H = 1.0;
    F G = exp2(-H);
    F f = 1.0;
    F a = 1.0;
    F t = 0.0;
    for( int i=0; i<number_octaves; i++){
        t += a*value_noise(f*x, seed);
        f *= 2.0;
        a *= G;
    }
    return t;
}	
	