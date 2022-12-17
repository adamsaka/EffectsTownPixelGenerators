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


*********************************************************************************************************/
#pragma once


/**************************************************************************************************
* Concept for all our SIMD types.
*************************************************************************************************/
template <typename T>
concept Simd = requires (T t) {
	//Public Elements
	t.v;

	//Information Functions.
	t.size_of_element();		//Sizeof an underlying single element (sizeof(T::F))
	t.number_of_elements();		//Number of elements.


	//Typedefs
	T::F;						//Indicates the type of the underlying type for a single element.

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
};



/**************************************************************************************************
* Concept for types that are based on floating point (any precision).
*************************************************************************************************/
template <typename T>
concept SimdFloat = (SimdFloat32<T> || SimdFloat64<T>) && requires (T t) {
	T::U;						//Indicates the unsined int size of the same size.
	T::U64;						//Indicates the unsigned int of 64-bit size.

	//Operators
	-t;

};

/**************************************************************************************************
* Concept for types that are based on unsigned ints (any size).
*************************************************************************************************/
template <typename T>
concept SimdUint = (SimdUint32<T> || SimdUint64<T>) && requires (T t) {

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

};


/**************************************************************************************************
* Concept for types that are based on 64-bit floating point.
*************************************************************************************************/
template <typename T>
concept SimdFloat64 = Simd<T> && requires (T t) {
	{t.element(0)} -> std::same_as<double>;
		requires sizeof(t.element(0)) == 8;
};



/**************************************************************************************************
* Concept for types that are based on 32-bit floating point.
*************************************************************************************************/
template <typename T>
concept SimdFloat32 = Simd<T> && requires (T t) {
	{t.element(0)} -> std::same_as<float>;
		requires sizeof(t.element(0)) == 4;
};

/**************************************************************************************************
* Concept for types that are based on 64-bit unsigned ints
*************************************************************************************************/
template <typename T>
concept SimdUInt64 = Simd<T> && requires (T t) {
	{t.element(0)} -> std::same_as<uint64_t>;
		requires sizeof(t.element(0)) == 8;
};

/**************************************************************************************************
* Concept for types that are based on 32-bit unsigned ints
*************************************************************************************************/
template <typename T>
concept SimdUInt32 = Simd<T> && requires (T t) {
	{t.element(0)} -> std::same_as<uint32_t>;
	requires sizeof(t.element(0)) == 4;
};
