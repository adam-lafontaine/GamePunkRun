#pragma once

#include <cstdint>
#include <cstddef>
#include <cassert>

using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

using i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;

using f32 = float;
using f64 = double;

using b32 = u32;
using b8 = u8;

using uangle = u16;
using iangle = i16;

using cstr = const char*;


template <typename T>
class Result
{
public:
	bool ok = false;
	T data;
};


template <typename T>
class Vec2D
{
public:
	T x;
	T y;
};


template <typename T>
class Rect2D
{
public:
	T x_begin;
	T x_end;
	T y_begin;
	T y_end;
};


template <typename T>
class Matrix2D
{
public:
	T* data_ = nullptr;
	u32 width = 0;
	u32 height = 0;
};


template <typename T>
class MatrixView2D
{
public:

	T* matrix_data_ = nullptr;

	u32 width = 0;
	u32 height = 0;
};


template <typename T>
using Point2D = Vec2D<T>;

/*template <typename T>
class Point2D
{
public:
	T x;
	T y;
};*/

using Vec2Di8 = Vec2D<i8>;
using Vec2Du8 = Vec2D<u8>;
using Vec2Di32 = Vec2D<i32>;
using Vec2Df32 = Vec2D<f32>;
using Vec2Du32 = Vec2D<u32>;
using Vec2Di64 = Vec2D<i64>;
//using Vec2Du64 = Vec2D<u64>;
//using Vec2Df64 = Vec2D<f64>;

using Point2Di32 = Point2D<i32>;
//using Point2Df32 = Point2D<f32>;
using Point2Du32 = Point2D<u32>;
using Point2Du64 = Point2D<u64>;
using Point2Di64 = Point2D<i64>;
//using Point2Df64 = Point2D<f64>;

using Rect2Di32 = Rect2D<i32>;
//using Rect2Df32 = Rect2D<f32>;
using Rect2Du32 = Rect2D<u32>;
//using Rect2Di64 = Rect2D<i64>;
//using Rect2Df64 = Rect2D<f64>;



template <typename T>
class Circle2D
{
public:
	Point2D<T> center;
	T radius;	
};

using Circle2Df32 = Circle2D<f32>;


//#define CAN_TBB

#ifdef CAN_TBB

#if __has_include(<tbb/parallel_for.h>)
#include <tbb/parallel_for.h>
#define HAS_TBB
#endif

#endif