#pragma once

// A header file to get you set going with Intel SIMD instrinsic programming.
// All necessary header files are inlucded for SSE2, SSE41, and AVX2
// Macros make the intrinsics easier to read and  generic so you can compile to
// SSE2 or AVX2 with the flip of a #define

#define SIMD_SSE2  //indicates we want SSE2
#define SIMD_SSE41 //indicates we want SSE4.1 instructions (floor and blend is available)
#define SIMD_AVX2  //indicates we want AVX2 instructions (double speed!)

#ifndef SIMD_AVX2
#include <xmmintrin.h> //SSE
#include <emmintrin.h> //SSE 2
#endif

#ifdef SIMD_SSE41
#include <smmintrin.h> // SSE4.1
#endif

#ifdef SIMD_AVX2
#include <immintrin.h> //avx2
#endif

// #include <zmmintrin.h> //avx512 the world is not yet ready...SOON

// create types we can use in either the 128 or 256 case
#ifndef SIMD_AVX2
// m128 will be our base type
typedef __m128 SIMD_Float;    //for floats
typedef __m128i SIMD_Integer; //for integers
typedef __m128d SIMD_Double;  //for doubles

// we process 4 at a time
#define SIMD_VECTOR_SIZE 4
#define SIMD_MEMORY_ALIGNMENT 16

// intrinsic functions
#define SIMD_Store(x, y) _mm_store_pd(x, y)
#define SIMD_Load(x) _mm_load_pd(x)
#define SIMD_SetOne(x) _mm_set1_pd(x)
#define SIMD_SetZero() _mm_setzero_pd()
#define SIMD_SetOnei(x) _mm_set1_epi64(x)
#define SIMD_SetZeroi(x) _mm_setzero_epi64(x)
#define SIMD_Add(x, y) _mm_add_pd(x, y)
#define SIMD_Sub(x, y) _mm_sub_pd(x, y)
#define SIMD_Addi(x, y) _mm_add_epi64(x, y)
#define SIMD_Subi(x, y) _mm_sub_epi64(x, y)
#define SIMD_Mul(x, y) _mm_mul_pd(x, y)
#define SIMD_Muli(x, y) _mm_mul_epi64(x, y)
#define SIMD_And(x, y) _mm_and_pd(x, y)
#define SIMD_Andi(x, y) _mm_and_si128(x, y)
#define SIMD_AndNot(x, y) _mm_andnot_pd(x, y)
#define SIMD_Or(x, y) _mm_or_pd(x, y)
#define SIMD_Ori(x, y) _mm_or_si128(x, y)
#define SIMD_CastToFloat(x) _mm_castsi128_pd(x)
#define SIMD_CastToInt(x) _mm_castpd_si128(x)
#define SIMD_ConvertToInt(x) _mm_cvtpd_epi64(x)
#define SIMD_ConvertToFloat(x) _mm_cvtepi64_pd(x)
#define SIMD_Equal(x, y) _mm_cmpeq_pd(x, y)
#define SIMD_Equali(x, y) _mm_cmpeq_epi64(x, y)
#define SIMD_GreaterThan(x, y) _mm_cmpgt_pd(x, y)
#define SIMD_GreaterThani(x, y) _mm_cmpgt_epi64(x, y)
#define SIMD_GreaterThanOrEq(x, y) _mm_cmpge_pd(x, y)
#define SIMD_LessThan(x, y) _mm_cmplt_pd(x, y)
#define SIMD_LessThani(x, y) _mm_cmpgt_epi64(y, x)
#define SIMD_LessThanOrEq(x, y) _mm_cmple_pd(x, y)
#define SIMD_NotEqual(x, y) _mm_cmpneq_pd(x, y)
#ifdef SIMD_SSE41
#define SIMD_Floor(x) _mm_floor_pd(x)
#endif
#define SIMD_Max(x, y) _mm_max_pd(x, y)
#define SIMD_Maxi(x, y) _mm_max_epi64(x, y)
#define SIMD_Min(x, y) _mm_min_pd(x, y)
#ifndef SIMD_SSE41
#define SIMD_BlendV(x, y, z) Or(AndNot(z, x), And(z, y)) //if we don't have sse4
#else
#define SIMD_BlendV(x, y, z) _mm_blendv_pd(x, y, z)
#endif

#endif

#ifdef SIMD_AVX2

// m256 will be our base type
typedef __m256 SIMD_Float;    //for floats
typedef __m256i SIMD_Integer; //for integers
typedef __m256d SIMD_Double;  //for doubles

//process 8 at t time
#define SIMD_VECTOR_SIZE 8
#define SIMD_MEMORY_ALIGNMENT 64

//intrinsic functions
#define SIMD_Store(x, y) _mm256_store_pd(x, y)
#define SIMD_Load(x) _mm256_load_pd(x)
#define SIMD_Set(x, y, z, w) _mm256_set_pd(x, y, z, w);
#define SIMD_SetOne(x) _mm256_set1_pd(x)
#define SIMD_SetZero() _mm256_setzero_pd()
#define SIMD_SetOnei(x) _mm256_set1_epi64x(x)
#define SIMD_SetZeroi(x) _mm256_setzero_epi64(x)
#define SIMD_SetZero256i() _mm256_setzero_si256()
#define SIMD_Add(x, y) _mm256_add_pd(x, y)
#define SIMD_Sub(x, y) _mm256_sub_pd(x, y)
#define SIMD_Addi(x, y) _mm256_add_epi64(x, y)
#define SIMD_Subi(x, y) _mm256_sub_epi64(x, y)
#define SIMD_Mul(x, y) _mm256_mul_pd(x, y)
#define SIMD_Muli(x, y) _mm256_mul_epi64(x, y)
#define SIMD_MulAdd(x, y, z) _mm256_fmadd_pd(x, y, z)
#define SIMD_And(x, y) _mm256_and_pd(x, y)
#define SIMD_Andi(x, y) _mm256_and_si256(x, y)
#define SIMD_AndNot(x, y) _mm256_andnot_pd(x, y)
#define SIMD_Or(x, y) _mm256_or_pd(x, y)
#define SIMD_Ori(x, y) _mm256_or_si256(x, y)
#define SIMD_CastToFloat(x) _mm256_castsi256_pd(x)
#define SIMD_CastToInt(x) _mm256_castpd_si256(x)
#define SIMD_ConvertToInt(x) _mm256_cvtpd_epi64(x)
#define SIMD_ConvertToFloat(x) _mm256_cvtepi64_pd(x)
#define SIMD_Equal(x, y) _mm256_cmp_pd(x, y, _CMP_EQ_OQ)
#define SIMD_Equali(x, y) _mm256_cmpeq_epi64(x, y)
#define SIMD_GreaterThan(x, y) _mm256_cmp_pd(x, y, _CMP_GT_OQ)
#define SIMD_GreaterThani(x, y) _mm256_cmpgt_epi64(x, y)
#define SIMD_LessThan(x, y) _mm256_cmp_pd(x, y, _CMP_LT_OQ)
#define SIMD_LessThani(x, y) _mm256_cmpgt_epi64(y, x)
#define SIMD_LessThanOrEq(x, y) _mm256_cmp_pd(x, y, _CMP_LE_OQ)
#define SIMD_GreaterThanOrEq(x, y) _mm256_cmp_pd(x, y, _CMP_GE_OQ)
#define SIMD_NotEqual(x, y) _mm256_cmp_pd(x, y, _CMP_NEQ_OQ)
#define SIMD_SignMask(x) _mm256_movemask_pd(x)
#define SIMD_Floor(x) _mm256_floor_pd(x)
#define SIMD_Max(x, y) _mm256_max_pd(x, y)
#define SIMD_Maxi(x, y) _mm256_max_epi64(x, y)
#define SIMD_Min(x, y) _mm256_min_pd(x, y)
#define SIMD_Gather(x, y, z) _mm256_i64gather_epi64(x, y, z)
#define SIMD_Gatherf(x, y, z) _mm256_i64gather_pd(x, y, z)
#define SIMD_BlendV(x, y, z) _mm256_blendv_pd(x, y, z)
#endif