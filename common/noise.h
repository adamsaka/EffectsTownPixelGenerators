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
#include "simd-f32.h"
#include "simd-f64.h"
#include "simd-uint32.h"
#include "simd-uint64.h"
#include "simd-concepts.h"

#include <concepts>
#include <cmath>
#include <limits>
#include <bit>
#include <immintrin.h>
#include <concepts>


/**************************************************************************************************
 * "SplitMix64" random number generator
 * (Public Domain)
 * https://rosettacode.org/wiki/Pseudo-random_numbers/Splitmix64
 * ************************************************************************************************/
template <typename U64> requires std::same_as<U64, uint64_t> || SimdUInt64<U64>
static inline constexpr U64 split_mix_64(U64 state) {
    U64 z = state + 0x9e3779b97f4a7c15;
    z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9;
    z = (z ^ (z >> 27)) * 0x94d049bb133111eb;
    return z ^ (z >> 31);
}

/****************************************************************************std::same_as<F, double>**********************
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



/***************************************************************************************************
* 32-bit hashing function 
* (derived from murmer3, public domain)
* https://github.com/aappleby/smhasher/blob/master/src/MurmurHash3.cpp
**************************************************************************************************/

template <typename U32, typename S32> requires (std::same_as<U32, uint32_t> || SimdUInt32<U32>) && (std::same_as<S32, uint32_t> || SimdUInt32<S32>)
static inline U32 hash_32(U32 data, S32 seed) {
    data *= 0xcc9e2d51;
    data = (data << 15) | (data >> 17);
    data *= 0x1b873593;
    data ^= seed;
    data = (data << 13) | (data >> 19);
    //data = data * 5 + 0xe6546b64;
    return data;
}

template <typename U32> requires std::same_as<U32, uint32_t> || SimdUInt32<U32>
static inline U32 hash_32_final(U32 data) {
    //data *= 0xcc9e2d51;
    //data = (data << 15) | (data >> 17);
    //data *= 0x1b873593;
    //data ^= data >> 16;
    data *= 0x85ebca6b;
    data ^= data >> 13;
    data *= 0xc2b2ae35;
    data ^= data >> 16;
    return data;
}






/**************************************************************************************************
 * Bitcasting of floats to ints
 * ************************************************************************************************/
static inline constexpr uint64_t cast_to_int64(float f) {
    return static_cast<uint64_t>(std::bit_cast<uint32_t>(f));
}
static inline constexpr uint64_t cast_to_int64(double f){
    return std::bit_cast<uint64_t>(f);
}

/**************************************************************************************************
 * Hash based on coordinates and a seed value
 * ************************************************************************************************/
template <SimdFloat32 S>
static inline  S hash(const S& coordinate, uint32_t seed ){
    auto r = hash_32(coordinate.bitcast_to_uint32(), seed);
    r = hash_32_final(r);
    auto result = S::make_from_int32(r >> 9) / S(0xffffffff >> 9);
    return result;
}

template<SimdFloat32 S>
static inline S hash(const vec2<S>& coordinate, uint32_t seed ) {    
    //auto r = hash_32(coordinate.x.bitcast_to_uint32(), coordinate.y.bitcast_to_uint32(), seed);
    auto r = hash_32(coordinate.x.bitcast_to_uint32(), seed);
    r = hash_32(coordinate.y.bitcast_to_uint32(), r);
    r = hash_32_final(r);
    auto result = S::make_from_int32(r>>9) / S(0xffffffff >> 9); 
    return result;
}

template<SimdFloat32 S>
static inline S hash(const vec3<S>& coordinate, uint32_t seed ) {
    auto r = hash_32(coordinate.x.bitcast_to_uint32(), seed);
    r = hash_32(coordinate.y.bitcast_to_uint32(), r);
    r = hash_32(coordinate.z.bitcast_to_uint32(), r);
    r = hash_32_final(r);
    auto result = S::make_from_int32(r >> 9) / S(0xffffffff >> 9);
    return result;
}

template<SimdFloat32 S>
static inline S hash(const vec4<S>& coordinate, uint32_t seed ) {
    auto r = hash_32(coordinate.x.bitcast_to_uint32(), seed);
    r = hash_32(coordinate.y.bitcast_to_uint32(), r);
    r = hash_32(coordinate.z.bitcast_to_uint32(), r);
    r = hash_32(coordinate.w.bitcast_to_uint32(), r);
    r = hash_32_final(r);
    auto result = S::make_from_int32(r >> 9) / S(0xffffffff >> 9);
    return result;
}

template <SimdFloat64 S>
static inline S hash(const vec4<S> & coordinate, uint64_t seed = 1){
    auto seed64 = S::U64(seed);
    seed64 ^= coordinate.x.bitcast_to_uint64();
    seed64 ^= rotr(coordinate.y.bitcast_to_uint64(), 16);
    seed64 ^= rotr(coordinate.z.bitcast_to_uint64(), 32);
    seed64 ^= rotr(coordinate.w.bitcast_to_uint64(), 48);
        
    auto r = split_mix_64(seed64);
    auto f = S::make_from_uints_52bits(r); 

    return f / S(static_cast<double>(0b0000000000001111111111111111111111111111111111111111111111111111 ));
}

//AVX-512 Specialization. It is worth using intrinsics on AVX-512 as it has integer multiply
static inline Simd512Float64 hash(const vec4<Simd512Float64>& coordinate, uint64_t seed = 1) {
    typedef Simd512Float64 S;

    auto seed64 = S::U64(seed);
    seed64 ^= coordinate.x.bitcast_to_uint64();
    seed64 ^= rotr(coordinate.y.bitcast_to_uint64(), 16);
    seed64 ^= rotr(coordinate.z.bitcast_to_uint64(), 32);
    seed64 ^= rotr(coordinate.w.bitcast_to_uint64(), 48);

    auto r = split_mix_64(seed64);
    auto f = S::make_from_uints_52bits(r);

    return f / S(static_cast<double>(0b0000000000001111111111111111111111111111111111111111111111111111));
}











/**************************************************************************************************
 * Simple method to convert a string to a uint64 seed
 * ************************************************************************************************/
static inline constexpr uint32_t string_to_seed(const std::string& str){
    uint32_t seed {0};
    for (size_t i=0;i<str.length();i++){
        seed ^= static_cast<uint32_t>(str[i]) >> ((i & 4)*4); 
    }
    return seed;
}




/**************************************************************************************************
Value Noise
*************************************************************************************************/
template <typename F> requires SimdFloat<F> || std::floating_point<F>
static F value_noise(F p, uint32_t seed = 1) {
    F i = floor(p);
    F f = fract(p);  //Adjusting the negative values of fract makes smooth noise for negative inputs
    F u = f * f * (static_cast<F>(3.0) - (f + f));

    const F x1 = hash(i, seed);
    const F x2 = hash(i + 1.0, seed);
    

    return mix(x1,x2, u);

    
}


template <typename F> requires SimdFloat<F> || std::floating_point<F>
static F value_noise(vec2<F> p , uint32_t seed=1){
    vec2<F> i = floor(p);
    vec2<F> f = fract(p);  //Adjusting the negative values of fract makes smooth noise for negative inputs
    vec2<F> u = f*f*(static_cast<F>(3.0)- (f + f));
    
    const F x1 = hash(i + vec2<F>(0.0, 0.0), seed);
    const F x2 = hash(i + vec2<F>(1.0, 0.0), seed);
    const F x3 = hash(i + vec2<F>(0.0, 1.0), seed);
    const F x4 = hash(i + vec2<F>(1.0, 1.0), seed);
    
    return mix(mix(x1, x2, u.x), mix(x3, x4, u.x), u.y);

    //return mix(mix(x1 , x2 , u.x),mix(x3 , x4, u.y), u.x); -- keep for later cool effect
}


template <typename F> requires SimdFloat<F> || std::floating_point<F>
static F value_noise(vec3<F> p, uint32_t seed = 0) {
    vec3<F> i = floor(p);
    vec3<F> f = fract(p);  //Adjusting the negative values of fract makes smooth noise for negative inputs
    vec3<F> u = f * f * (static_cast<F>(3.0) - (f + f));

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

template <typename F> requires SimdFloat<F> || std::floating_point<F>
static F value_noise(vec4<F> p, uint32_t seed = 0) {
    vec4<F> i = floor(p);
    vec4<F> f = fract(p);  
    vec4<F> u = f * f * (static_cast<F>(3.0) - (f + f));

    const F x1 = hash<F>(i + vec4<F>(0.0, 0.0, 0.0, 0.0), seed);
    const F x2 = hash<F>(i + vec4<F>(1.0, 0.0, 0.0, 0.0), seed);
    const F y1 = mix(x1, x2, u.x);

    const F x3 = hash<F>(i + vec4<F>(0.0, 1.0, 0.0, 0.0), seed);
    const F x4 = hash<F>(i + vec4<F>(1.0, 1.0, 0.0, 0.0), seed);
    const F y2 = mix(x3, x4, u.x);
    const F z1 = mix(y1, y2, u.y);

    const F x5 = hash<F>(i + vec4<F>(0.0, 0.0, 1.0, 0.0), seed);
    const F x6 = hash<F>(i + vec4<F>(1.0, 0.0, 1.0, 0.0), seed);
    const F y3 = mix(x5, x6, u.x);

    const F x7 = hash<F>(i + vec4<F>(0.0, 1.0, 1.0, 0.0), seed);
    const F x8 = hash<F>(i + vec4<F>(1.0, 1.0, 1.0, 0.0), seed);
    const F y4 = mix(x7, x8, u.x);
    const F z2 = mix(y3, y4, u.y);
    const F w1 = mix(z1, z2, u.z);
    
    const F x9 = hash<F>(i + vec4<F>(0.0, 0.0, 0.0, 1.0), seed);
    const F x10 = hash<F>(i + vec4<F>(1.0, 0.0, 0.0, 1.0), seed);
    const F y5 = mix(x9, x10, u.x);

    const F x11 = hash<F>(i + vec4<F>(0.0, 1.0, 0.0, 1.0), seed);
    const F x12 = hash<F>(i + vec4<F>(1.0, 1.0, 0.0, 1.0), seed);
    const F y6 = mix(x11, x12, u.x);
    const F z3 = mix(y5, y6, u.y);
    
    const F x13 = hash<F>(i + vec4<F>(0.0, 0.0, 1.0, 1.0), seed);
    const F x14 = hash<F>(i + vec4<F>(1.0, 0.0, 1.0, 1.0), seed);
    const F y7 = mix(x13, x14, u.x);

    const F x15 = hash<F>(i + vec4<F>(0.0, 1.0, 1.0, 1.0), seed);
    const F x16 = hash<F>(i + vec4<F>(1.0, 1.0, 1.0, 1.0), seed);
    const F y8 = mix(x15, x16, u.x);
    const F z4 = mix(y7, y8, u.y);
    const F w2 = mix(z3, z4, u.z);    
    

    return mix(w1, w2, u.w);
}


/**************************************************************************************************
Based on article by Inigo Quilez https://www.iquilezles.org/www/articles/fbm/fbm.htm
The original code snippet was released under the MIT license: https://opensource.org/licenses/MIT
*************************************************************************************************/
template <typename F> requires SimdFloat<F> || std::floating_point<F>
static F fbm(F x, int number_octaves = 8, uint32_t seed = 1) {
    const F G = exp2(-1.0f);

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

template <typename F> requires SimdFloat<F> || std::floating_point<F>
static F fbm(vec2<F> x, int number_octaves = 8, uint32_t seed=1){
    const F G = exp2(-1.0f);
    
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
template <typename F> requires SimdFloat<F> || std::floating_point<F>
static F fbm(vec3<F> x, int number_octaves = 8, uint32_t seed = 0) {
    const F::F H{ exp2(-1.0f) };
    F G = H;
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
template <typename F> requires SimdFloat<F> || std::floating_point<F>
static F fbm(vec4<F> x, int number_octaves = 8, uint32_t seed = 0) {
    const F::F H{ exp2( -1.0f)};
    F G = H;
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

