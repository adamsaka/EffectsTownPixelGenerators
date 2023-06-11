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


*********************************************************************************************************/
#pragma once

#include <concepts>


/**************************************************************************************************
* Concept for all our SIMD types.
* 
* These types may or may not actually use SIMD instructions. 
* Any type that implements the required concepts can be used.
* 
* 
*************************************************************************************************/
template <typename T>
concept Simd = requires (T t) {

	

	//Size of struct should be sizeof individual elements.
	requires sizeof(t.v) == sizeof(t.element(0)) * t.number_of_elements();
	requires sizeof(t) == sizeof(t.v);
	
	//General
	requires std::copy_constructible<T>;
	requires std::copyable<T>;
	requires std::movable<T>;
	
	//Has CPU Support functions
	T::compiler_level_supported();
	T::compiler_supported();
	T::cpu_supported();
	T::cpu_level_supported();
	
	//Public Elements
	t.v;

	//Constructors
	requires std::default_initializable<T>;
	requires std::constructible_from<T, decltype(t.v)>;
	requires std::constructible_from<T, decltype(t.element(0))>;


	//Information Functions.
	t.size_of_element();		//Sizeof an underlying single element (sizeof(T::F))
	t.number_of_elements();		//Number of elements.
	
	

	//Typedefs		
	static_cast<typename T::F>(1);  //T::F  must exist and by castable from a numberic type
	requires std::same_as<decltype(t.element(0)), decltype(static_cast<T::F>(1))>; //T::F must be same type as the elements.

	//Operators
	t + t;
	t - t;
	t * t;
	t / t;
	t += t;
	t -= t;
	t *= t;
	t /= t;
	

	//Element Access
	t.element(0);				//Read the value of an element  (index < number_of_elements() )

	//Make
	T::make_sequential(typename T::F(static_cast<typename T::F>(1)));
};


/**************************************************************************************************
* Concept for types that are signed
*
* Must implement "Simd" concept and the following:
* 
* Negation Operator.
* abs(T)
*
*************************************************************************************************/
template <typename T>
concept SimdSigned = Simd<T> && requires (T t) {
	//Operators
	-t;
	abs(t);
};




/**************************************************************************************************
* Concept for types that are real. (Could be fixed or floating point)
*
* Must implement "Simd" concept and the following:
* 
*
* floor(T)						
* ceil(T);
* trunc(T);
* round(T);
* fract(T);						Fractional Part.  (a - floor(a))
*
*************************************************************************************************/
template <typename T>
concept SimdReal = Simd<T> && SimdSigned<T> && requires (T t) {
	floor(t);
	ceil(t);
	trunc(t);
	round(t);
	fract(t);
	min(T(1.0), T(2.0));
	max(T(1.0), T(2.0));
	clamp(t);
	clamp(t, typename T::F(1.0), typename T::F(2.0));
	clamp(t, T(1.0), T(2.0));
};


/**************************************************************************************************
* Concept for types that are real. (Could be fixed or floating point)
* Indicate support for mathematical operations.
*************************************************************************************************/
template <typename T>
concept SimdMath = SimdReal<T> && requires (T t) {
	sqrt(t);
	sin(t);
	cos(t);
	tan(t);
	asin(t);
	acos(t);
	atan(t);
	atan2(t, t);
	sinh(t);
	cosh(t);
	tanh(t);
	asinh(t);
	acosh(t);
	atanh(t);
	sqrt(t);
	abs(t);
	pow(t, 2.0f);
	exp(t);
	exp2(t);
	exp10(t);
	expm1(t);
	log(t);
	log1p(t);
	log2(t);
	log10(t);
	cbrt(t);
	hypot(t, t);


};




/**************************************************************************************************
* Concept for types that are based on floating point (any precision).
*
* Must implement "Simd" concept and the following:
* 
* reciprocal_approx(T)			Returns an approximate reciprocal (or an exact one if the machine functions don't exist)
* this.reciprocal_approx()		Const Member function version of reciprocal_approx
* 
* fma(T,T,T)					Fused Multiply Add  (a*b+c)
* fms(T,T,T)					Fused Multiply Subtract (a*b-c)
* fnma(T,T,T)					Fused Negative Multiply Subtract (-a*b+c)
* fnms(T,T,T)					Fused Negative Multiply Subtract (-a*b-c)
* 
*
*************************************************************************************************/
template <typename T>
concept SimdFloat = Simd<T> && SimdSigned<T> && SimdReal<T> && requires (T t) {
	reciprocal_approx(t);
	t.reciprocal_approx();

	fma(T(1.0), T(2.0), t);
	fms(T(1.0), T(2.0), t);
	fnma(T(1.0), T(2.0), t);
	fnms(T(1.0), T(2.0), t);

};


/**************************************************************************************************
* Concept for types that are based on 64-bit floating point.
*
* Must implement "SimdUInt" concept and have elements of double (64-bit):
*************************************************************************************************/
template <typename T>
concept SimdFloat64 = SimdFloat<T> && requires (T t) {
	{t.element(0)} -> std::same_as<double>;
		requires sizeof(t.element(0)) == 8;

	
};



/**************************************************************************************************
* Concept for types that are based on 32-bit floating point.
*
* * Must implement "SimdUInt" concept and have elements of double (32-bit):
*************************************************************************************************/
template <typename T>
concept SimdFloat32 = SimdFloat<T> && requires (T t) {
	{t.element(0)} -> std::same_as<float>;
		requires sizeof(t.element(0)) == 4;

	
};




/**************************************************************************************************
* Concept for types that are based on unsigned ints (any size).
* 
* Must implement "Simd" concept and the following:
* 
* Required Operators:
* <<, >>						Shift Operators
* |, |=							Bitwise Or
* &, &=							Bitwise And
* ^, ^=							Bitwise Xor
* ~								Bitwise Not
* 
*************************************************************************************************/
template <typename T>
concept SimdUInt = Simd<T> && requires (T t) {
	//Operators
	t << 2;
	t >> 2;
	t = t & t;
	t = t | t;
	t = t ^ t;
	t = ~t;
	t &= 0xff;
	t |= 0xff;
	t ^= 0xff;
	rotl(t, 2);
	rotr(t, 4);
};


/**************************************************************************************************
* Concept for types that are based on 64-bit unsigned ints
* 
* Must implement "SimdUInt" concept and have elements of uint64_t:
*************************************************************************************************/
template <typename T>
concept SimdUInt64 = Simd<T> && SimdUInt<T> && requires (T t) {
	{t.element(0)} -> std::same_as<uint64_t>;
		requires sizeof(t.element(0)) == 8;
};

/**************************************************************************************************
* Concept for types that are based on 32-bit unsigned ints
* 
* Must implement "SimdUInt" concept and have elements of uint32_t:
*************************************************************************************************/
template <typename T>
concept SimdUInt32 = Simd<T> && SimdUInt<T> && requires (T t) {
	{t.element(0)} -> std::same_as<uint32_t>;
	requires sizeof(t.element(0)) == 4;
};


/**************************************************************************************************
* Concept for types that are based on 32-bit unsigned ints
*
* Must implement "SimdUInt" concept and have elements of uint32_t:
*************************************************************************************************/
template <typename T>
concept SimdFloatToInt = Simd<T> && SimdFloat<T> && requires (T t) {
	//Indicates the unsined int type size of the same size.  May not be safe to use.  User should check if it is supported
	//This allows users to test for support, then before making appropriate conversion.	
	static_cast<typename T::U>(1.0);

	//Casting operations.
	t.bitcast_to_uint();   //Returns type T::U. May not be safe to use.  User should check if it is supported. 

};




