//////////////////////////////////////////////////////////////////////////////
//
// F64VEC4.H
//
//  Declaration of the F64vec4 class. This class is similiar to the 
//  F32vec4 class, but contains four double-precision elements.
//
//  Version 1.0
//  Written by Zvi Devir, Intel MSL
//
//////////////////////////////////////////////////////////////////////////////

//
//   Copyright (c) 2001 Intel Corporation.
//
// Permition is granted to use, copy, distribute and prepare derivative works 
// of this library for any purpose and without fee, provided, that the above 
// copyright notice and this statement appear in all copies.  
// Intel makes no representations about the suitability of this library for 
// any purpose, and specifically disclaims all warranties. 
// See LEGAL.TXT for all the legal information.
//

#ifndef F64VEC4_H__
#define F64VEC4_H__

#include <dvec.h>


struct __m256d {
    __m128d vec1, vec2;
};  


class F64vec4 {
public:
    __m128d vec1, vec2;

public:
    /* Constructors */
    F64vec4() {}
    F64vec4(const F64vec4 &m) { vec1 = m.vec1; vec2 = m.vec2; }
    F64vec4(const __m256d &m) { vec1 = m.vec1; vec2 = m.vec2; }
    F64vec4(const __m128d &m1, const __m128d &m0) { vec1 = m0; vec2 = m1; }
    F64vec4(const __m128d &m) { vec1 = vec2 = m; }
    F64vec4(double d3, double d2, double d1, double d0) { vec1 = _mm_set_pd(d1,d0); vec2 = _mm_set_pd(d3,d2); }
    F64vec4(double d) { vec1 = _mm_set_sd(d); vec1 = _mm_unpacklo_pd(vec1,vec1); vec2 = vec1; }

    operator __m256d() const { return *(__m256d*)&vec1; }
    F64vec4& operator = (const F64vec4 &a) { vec1 = a.vec1; vec2 = a.vec2; return *this; }

    /* Logical Operators */
    friend F64vec4 operator &(const F64vec4 &a, const F64vec4 &b) { return F64vec4(_mm_and_pd(a.vec2,b.vec2),_mm_and_pd(a.vec1,b.vec1)); }
    friend F64vec4 operator |(const F64vec4 &a, const F64vec4 &b) { return F64vec4(_mm_or_pd(a.vec2,b.vec2),_mm_or_pd(a.vec1,b.vec1)); }
    friend F64vec4 operator ^(const F64vec4 &a, const F64vec4 &b) { return F64vec4(_mm_xor_pd(a.vec2,b.vec2),_mm_xor_pd(a.vec1,b.vec1)); }
    friend F64vec4 andnot(const F64vec4 &a, const F64vec4 &b) { return F64vec4(_mm_andnot_pd(a.vec2,b.vec2),_mm_andnot_pd(a.vec1,b.vec1)); }

    /* Arithmetic Operators */
    friend F64vec4 operator +(const F64vec4 &a, const F64vec4 &b) { return F64vec4(_mm_add_pd(a.vec2,b.vec2),_mm_add_pd(a.vec1,b.vec1)); }
    friend F64vec4 operator -(const F64vec4 &a, const F64vec4 &b) { return F64vec4(_mm_sub_pd(a.vec2,b.vec2),_mm_sub_pd(a.vec1,b.vec1)); }
    friend F64vec4 operator *(const F64vec4 &a, const F64vec4 &b) { return F64vec4(_mm_mul_pd(a.vec2,b.vec2),_mm_mul_pd(a.vec1,b.vec1)); }
    friend F64vec4 operator /(const F64vec4 &a, const F64vec4 &b) { return F64vec4(_mm_div_pd(a.vec2,b.vec2),_mm_div_pd(a.vec1,b.vec1)); }

    F64vec4& operator +=(F64vec4 &a) { vec1 = _mm_add_pd(vec1,a.vec1); vec2 = _mm_add_pd(vec2,a.vec2); return *this; }
    F64vec4& operator -=(F64vec4 &a) { vec1 = _mm_sub_pd(vec1,a.vec1); vec2 = _mm_sub_pd(vec2,a.vec2); return *this; }
    F64vec4& operator *=(F64vec4 &a) { vec1 = _mm_mul_pd(vec1,a.vec1); vec2 = _mm_mul_pd(vec2,a.vec2); return *this; }
    F64vec4& operator /=(F64vec4 &a) { vec1 = _mm_div_pd(vec1,a.vec1); vec2 = _mm_div_pd(vec2,a.vec2); return *this; }
    F64vec4& operator &=(F64vec4 &a) { vec1 = _mm_and_pd(vec1,a.vec1); vec2 = _mm_and_pd(vec2,a.vec2); return *this; }
    F64vec4& operator |=(F64vec4 &a) { vec1 = _mm_or_pd(vec1,a.vec1); vec2 = _mm_or_pd(vec2,a.vec2); return *this; }
    F64vec4& operator ^=(F64vec4 &a) { vec1 = _mm_xor_pd(vec1,a.vec1); vec2 = _mm_xor_pd(vec2,a.vec2); return *this; }

    friend F64vec4 sqrt(const F64vec4 &a) { return F64vec4(_mm_sqrt_pd(a.vec2),_mm_sqrt_pd(a.vec1)); }

    /* Min and Max */
    friend F64vec4 simd_min(const F64vec4 &a, const F64vec4 &b) { return F64vec4(_mm_min_pd(a.vec2,b.vec2),_mm_min_pd(a.vec1,b.vec1)); }
    friend F64vec4 simd_max(const F64vec4 &a, const F64vec4 &b) { return F64vec4(_mm_max_pd(a.vec2,b.vec2),_mm_max_pd(a.vec1,b.vec1)); }
    
#if defined(_INC_IOSTREAM) || defined(_IOSTREAM_H_)
    /* Output */
    friend ostream & operator<<(ostream & os, const F64vec4 &a) 
    {
    /* To use: cout << "Elements of F64vec4 fvec are: " << fvec; */ 
        double *dp = (double*)&a;
        os << "[3]:" << *(dp+3) 
           << " [2]:" << *(dp+2) 
           << " [1]:" << *(dp+1) 
           << " [0]:" << *dp;
        return os;
    }
#endif

    /* Element Access Only, no modifications to elements*/
    const double& operator[] (int i) const
    {
        assert((0<=i) && (i<=3));
        return *(((double*)&vec1) + i);
    }
    /* Element Access and Modification*/
    double& operator[] (int i)
    {
        assert((0<=i) && (i<=3));
        return *(((double*)&vec1) + i);
    }

};



#endif // F64VEC4_H__