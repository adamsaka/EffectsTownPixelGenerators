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

    Noise, random and hashing functions.


*******************************************************************************************************/



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
    vec2<F> i = floor(p);
    vec2<F> f = fract_negative_adjust(p);  //Adjusting the negative values of fract makes smooth noise for negative inputs
    vec2<F> u = f*f*(static_cast<F>(3.0)- static_cast<F>(2.0)*f);
    
    const F x1 = hash(i + vec2<F>(0.0, 0.0), seed);
    const F x2 = hash(i + vec2<F>(1.0, 0.0), seed);
    const F x3 = hash(i + vec2<F>(0.0, 1.0), seed);
    const F x4 = hash(i + vec2<F>(1.0, 1.0), seed);
    
    return mix(mix(x1 , x2 , u.x),mix(x3 , x4, u.y), u.x);
}


template <std::floating_point F>
static F value_noise(vec3<F> p, uint64_t seed = 0) {
    vec3<F> i = floor(p);
    vec3<F> f = fract_negative_adjust(p);  //Adjusting the negative values of fract makes smooth noise for negative inputs
    vec3<F> u = f * f * (static_cast<F>(3.0) - static_cast<F>(2.0) * f);

    const F x1 = hash(i + vec3<F>(0.0, 0.0, 0.0), seed);
    const F x2 = hash(i + vec3<F>(1.0, 0.0, 0.0), seed);
    const F x3 = hash(i + vec3<F>(0.0, 1.0, 0.0), seed);
    const F x4 = hash(i + vec3<F>(1.0, 1.0, 0.0), seed);    
    const F x5 = hash(i + vec3<F>(0.0, 0.0, 1.0), seed);
    const F x6 = hash(i + vec3<F>(1.0, 0.0, 1.0), seed);
    const F x7 = hash(i + vec3<F>(0.0, 1.0, 1.0), seed);
    const F x8 = hash(i + vec3<F>(1.0, 1.0, 1.0), seed);
    
    const F y1 = mix(x1, x2, u.x);
    const F y2 = mix(x3, x4, u.x);
    const F y3 = mix(x5, x6, u.x);
    const F y4 = mix(x7, x8, u.x);

    const F z1 = mix(y1, y2, u.y);
    const F z2 = mix(y3, y4, u.y);

    return mix(z1, z2, u.z);
}

template <std::floating_point F>
static F value_noise(vec4<F> p, uint64_t seed = 0) {
    vec4<F> i = floor(p);
    vec4<F> f = fract_negative_adjust(p);  //Adjusting the negative values of fract makes smooth noise for negative inputs
    vec4<F> u = f * f * (static_cast<F>(3.0) - static_cast<F>(2.0) * f);

    const F x1 = hash(i + vec4<F>(0.0, 0.0, 0.0, 0.0), seed);
    const F x2 = hash(i + vec4<F>(1.0, 0.0, 0.0, 0.0), seed);
    const F x3 = hash(i + vec4<F>(0.0, 1.0, 0.0, 0.0), seed);
    const F x4 = hash(i + vec4<F>(1.0, 1.0, 0.0, 0.0), seed);
    const F x5 = hash(i + vec4<F>(0.0, 0.0, 1.0, 0.0), seed);
    const F x6 = hash(i + vec4<F>(1.0, 0.0, 1.0, 0.0), seed);
    const F x7 = hash(i + vec4<F>(0.0, 1.0, 1.0, 0.0), seed);
    const F x8 = hash(i + vec4<F>(1.0, 1.0, 1.0, 0.0), seed);
    const F x9 = hash(i + vec4<F>(0.0, 0.0, 0.0, 1.0), seed);
    const F x10 = hash(i + vec4<F>(1.0, 0.0, 0.0, 1.0), seed);
    const F x11 = hash(i + vec4<F>(0.0, 1.0, 0.0, 1.0), seed);
    const F x12 = hash(i + vec4<F>(1.0, 1.0, 0.0, 1.0), seed);
    const F x13 = hash(i + vec4<F>(0.0, 0.0, 1.0, 1.0), seed);
    const F x14 = hash(i + vec4<F>(1.0, 0.0, 1.0, 1.0), seed);
    const F x15 = hash(i + vec4<F>(0.0, 1.0, 1.0, 1.0), seed);
    const F x16 = hash(i + vec4<F>(1.0, 1.0, 1.0, 1.0), seed);

    const F y1 = mix(x1, x2, u.x);
    const F y2 = mix(x3, x4, u.x);
    const F y3 = mix(x5, x6, u.x);
    const F y4 = mix(x7, x8, u.x);
    const F y5 = mix(x9, x10, u.x);
    const F y6 = mix(x11, x12, u.x);
    const F y7 = mix(x13, x14, u.x);
    const F y8 = mix(x15, x16, u.x);

    const F z1 = mix(y1, y2, u.y);
    const F z2 = mix(y3, y4, u.y);
    const F z3 = mix(y5, y6, u.y);
    const F z4 = mix(y7, y8, u.y);

    const F w1 = mix(z1, z2, u.z);
    const F w2 = mix(z3, z4, u.z);

    return mix(w1, w2, u.w);
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
template <std::floating_point F>
static F fbm(vec3<F> x, int number_octaves = 8, uint64_t seed = 0) {
    const F H = 1.0;
    F G = exp2(-H);
    F f = 1.0;
    F a = 1.0;
    F t = 0.0;
    for (int i = 0; i < number_octaves; i++) {
        t += a * value_noise(f * x, seed);
        f *= 2.0;
        a *= G;
    }
    return t;
}
template <std::floating_point F>
static F fbm(vec4<F> x, int number_octaves = 8, uint64_t seed = 0) {
    const F H = 1.0;
    F G = exp2(-H);
    F f = 1.0;
    F a = 1.0;
    F t = 0.0;
    for (int i = 0; i < number_octaves; i++) {
        t += a * value_noise(f * x, seed);
        f *= 2.0;
        a *= G;
    }
    return t;
}

