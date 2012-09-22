//////////////////////////////////////////////////////////////////////////////
//
// SPMATRIX.CPP
//
//  Implementation of the SPMatrix, SPVector and the SPVector3 classes.
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
// Intel makes no representations about the suitability of this software for 
// any purpose, and specifically disclaims all warranties. 
// See LEGAL.TXT for all the legal information.
//


#include "SPMatrix.h"
#ifdef USE_AMLIB
#include "amaths.h"
#endif


const _MM_ALIGN16 __int32 __MASKSIGNs_[4] = { 0x80000000, 0x80000000, 0x80000000, 0x80000000 };
const _MM_ALIGN16 __int32 _Sign_PNPN[4] = { 0x00000000, 0x80000000, 0x00000000, 0x80000000 };
const _MM_ALIGN16 __int32 _Sign_NPNP[4] = { 0x80000000, 0x00000000, 0x80000000, 0x00000000 };
const _MM_ALIGN16 __int32 _Sign_PNNP[4] = { 0x00000000, 0x80000000, 0x80000000, 0x00000000 };
const _MM_ALIGN16 __int32 __0FFF_[4] = { 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000 };
const _MM_ALIGN16 float __ZERONE_[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
const _MM_ALIGN16 float __1000_[4] = { 1.0f, 0.0f, 0.0f, 0.0f };

#define _MASKSIGNs_ (*(F32vec4*)&__MASKSIGNs_)      // - - - -
#define Sign_PNPN   (*(F32vec4*)&_Sign_PNPN)        // + - + -
#define Sign_NPNP   (*(F32vec4*)&_Sign_NPNP)        // - + - +
#define Sign_PNNP   (*(F32vec4*)&_Sign_PNNP)        // + - - +
#define _0FFF_      (*(F32vec4*)&__0FFF_)           // 0 * * *
#define _ZERONE_    (*(F32vec4*)&__ZERONE_)         // 1 0 0 1
#define _1000_      (*(F32vec4*)&__1000_)           // 1 0 0 0


#ifdef USE_AMLIB
#define M_PI 3.14159265358979323846f
const _MM_ALIGN16 float __PIsY_[4] = { 0.0f, M_PI*0.5f, M_PI*0.5f, -M_PI };
const _MM_ALIGN16 float __PIsXZ_[4] = { -M_PI, M_PI*0.5f, M_PI*0.5f, 0.0f };

#define _PIsY_      (*(F32vec4*)&__PIsY_)           // -sin cos cos sin
#define _PIsXZ_     (*(F32vec4*)&__PIsXZ_)          // sin cos cos -sin
#endif


            /***********************************************************/
            /*                Matrix Constructors                      */
            /***********************************************************/

// ----------------------------------------------------------------------------
//  Name:   RotateXMatrix
//  Desc:   Returns the rotation matrix of X axis of specific angle.
// ----------------------------------------------------------------------------
void SPMatrix::RotateXMatrix(const float rads) {
    __asm { 
        xorps   xmm0,xmm0
        mov     eax, this;
        fld     float ptr rads
        movaps  [eax+0x10], xmm0        // clear line _L2
        fsincos
        fst     float ptr [eax+0x14]    // set element _22
        movaps  [eax+0x20], xmm0        // clear line _L3
        fstp    float ptr [eax+0x28]    // set element _33
        fst     float ptr [eax+0x18]    // set element _23
        fchs
        movaps  [eax+0x00], xmm0        // clear line _L1
        fstp    float ptr [eax+0x24]    // set element _32
        fld1
        fst     float ptr [eax+0x00]    // set element _11
        movaps  [eax+0x30], xmm0        // clear line _L4
        fstp    float ptr [eax+0x3C]    // set element _44
    }
} 
SPMatrix RotateXMatrix(const float rads) {
    SPMatrix ret;
    ret.RotateXMatrix(rads);
    return ret;
} 

#ifdef USE_AMLIB
void SPMatrix::RotateXMatrixA(const float rads) {
    F32vec4 cossin = F32vec4(rads)+_PIsXZ_;
    _L1 = _L2 = _L3 = _L4 = _mm_setzero_ps();
    cossin = am_sin_ps(cossin); _m_empty();
    _11 = _44 = 1.0f;
    _mm_storeh_pi((__m64 *)&_22,cossin);
    _mm_storel_pi((__m64 *)&_32,cossin);
} 
SPMatrix RotateXMatrixA(const float rads) {
    SPMatrix ret;
    F32vec4 cossin = F32vec4(rads)+_PIsXZ_;
    ret._L1 = ret._L2 = ret._L3 = ret._L4 = _mm_setzero_ps();
    cossin = am_sin_ps(cossin); _m_empty();
    ret._11 = ret._44 = 1.0f;
    _mm_storeh_pi((__m64 *)&ret._22,cossin);
    _mm_storel_pi((__m64 *)&ret._32,cossin);
    return ret;
} 
#endif

// ----------------------------------------------------------------------------
//  Name:   RotateYMatrix
//  Desc:   Returns the rotation matrix of Y axis of specific angle.
// ----------------------------------------------------------------------------
void SPMatrix::RotateYMatrix(const float rads) {
    __asm { 
        xorps   xmm0,xmm0
        mov     eax, this
        fld     float ptr rads
        movaps  [eax+0x00], xmm0        // clear line _L1
        fsincos
        fst     float ptr [eax+0x00]    // set element _11
        movaps  [eax+0x20], xmm0        // clear line _L3
        fstp    float ptr [eax+0x28]    // set element _33
        fst     float ptr [eax+0x20]    // set element _31
        fchs
        movaps  [eax+0x10], xmm0        // clear line _L2
        fstp    float ptr [eax+0x08]    // set element _13
        fld1
        fst     float ptr [eax+0x14]    // set element _22
        movaps  [eax+0x30], xmm0        // clear line _L4
        fstp    float ptr [eax+0x3C]    // set element _44
    }
} 
SPMatrix RotateYMatrix(const float rads) {
    SPMatrix ret;
    ret.RotateYMatrix(rads);
    return ret;
} 

#ifdef USE_AMLIB
void SPMatrix::RotateYMatrixA(const float rads) {
    F32vec4 cossin = F32vec4(rads)+_PIsY_;
    _L2 = _L4 = _mm_setzero_ps();
    cossin = am_sin_ps(cossin); _m_empty();
    _22 = _44 = 1.0f;
    _L1 = _mm_unpackhi_ps(cossin,_mm_setzero_ps());
    _L3 = _mm_unpacklo_ps(cossin,_mm_setzero_ps());
} 
SPMatrix RotateYMatrixA(const float rads) {
    SPMatrix ret;
    F32vec4 cossin = F32vec4(rads)+_PIsY_;
    cossin = am_sin_ps(cossin); _m_empty();
    ret._L2 = ret._L4 = _mm_setzero_ps();
    ret._22 = ret._44 = 1.0f;
    ret._L1 = _mm_unpackhi_ps(cossin,_mm_setzero_ps());
    ret._L3 = _mm_unpacklo_ps(cossin,_mm_setzero_ps());
    return ret;
} 
#endif

// ----------------------------------------------------------------------------
//  Name:   RotateZMatrix
//  Desc:   Returns the rotation matrix of Z axis of specific angle.
// ----------------------------------------------------------------------------
void SPMatrix::RotateZMatrix(const float rads) {
    __asm { 
        xorps   xmm0,xmm0
        mov     eax, this
        fld     float ptr rads
        movaps  [eax+0x00], xmm0        // clear line _L1
        fsincos
        fst     float ptr [eax+0x00]    // set element _11
        movaps  [eax+0x10], xmm0        // clear line _L2
        fstp    float ptr [eax+0x14]    // set element _22
        fst     float ptr [eax+0x04]    // set element _12
        fchs
        movaps  [eax+0x20], xmm0        // clear line _L3
        fstp    float ptr [eax+0x10]    // set element _21
        fld1
        fst     float ptr [eax+0x28]    // set element _33
        movaps  [eax+0x30], xmm0        // clear line _L4
        fstp    float ptr [eax+0x3C]    // set element _44
    }
} 
SPMatrix RotateZMatrix(const float rads) {
    SPMatrix ret;
    ret.RotateZMatrix(rads);
    return ret;
} 

#ifdef USE_AMLIB
void SPMatrix::RotateZMatrixA(const float rads) {
    F32vec4 cossin = F32vec4(rads)+_PIsXZ_;
    _L1 = _L2 = _L3 = _L4 = _mm_setzero_ps();
    cossin = am_sin_ps(cossin); _m_empty();
    _33 = _44 = 1.0f;
    _mm_storeh_pi((__m64 *)&_11,cossin);
    _mm_storel_pi((__m64 *)&_21,cossin);
} 
SPMatrix RotateZMatrixA(const float rads) {
    SPMatrix ret;
    F32vec4 cossin = F32vec4(rads)+_PIsXZ_;
    ret._L1 = ret._L2 = ret._L3 = ret._L4 = _mm_setzero_ps();
    cossin = am_sin_ps(cossin); _m_empty();
    ret._33 = ret._44 = 1.0f;
    _mm_storeh_pi((__m64 *)&ret._11,cossin);
    _mm_storel_pi((__m64 *)&ret._21,cossin);
    return ret;
} 
#endif

// ----------------------------------------------------------------------------
//  Name:   TranslateMatrix
//  Desc:   Returns the translation matrix for specific translation.
// ----------------------------------------------------------------------------
SPMatrix TranslateMatrix(const float dx, const float dy, const float dz)
{
    SPMatrix ret;
    ret.IdentityMatrix();
    ret._41 = dx;
    ret._42 = dy;
    ret._43 = dz;
    return ret;
} 
void SPMatrix::TranslateMatrix(const float dx, const float dy, const float dz)
{
    IdentityMatrix();
    _41 = dx;
    _42 = dy;
    _43 = dz;
} 

// ----------------------------------------------------------------------------
//  Name:   ScaleMatrix
//  Desc:   Returns the scaling matrix for x,y,z axis.
// ----------------------------------------------------------------------------
SPMatrix ScaleMatrix(const float a, const float b, const float c)
{
    SPMatrix ret;
    ret.IdentityMatrix();
    ret._11 = a;
    ret._22 = b;
    ret._33 = c;
    return ret;
} 
void SPMatrix::ScaleMatrix(const float a, const float b, const float c)
{
    IdentityMatrix();
    _11 = a;
    _22 = b;
    _33 = c;
} 

// ----------------------------------------------------------------------------
//  Name:   ScaleMatrix
//  Desc:   Returns the scaling matrix by scalar.
// ----------------------------------------------------------------------------
SPMatrix ScaleMatrix(const float a)
{
    SPMatrix ret;
    ret.IdentityMatrix();
    ret._11 = ret._22 = ret._33 = a;
    return ret;
} 
void SPMatrix::ScaleMatrix(const float a)
{
    IdentityMatrix();
    _11 = _22 = _33 = a;
} 


            /***********************************************************/
            /*                    Matrix Operators                     */
            /***********************************************************/

// ----------------------------------------------------------------------------
//  Name:   SPMatrix::MinValue
//  Desc:   Returns the asbolute minimum element of the matrix.
// ----------------------------------------------------------------------------
float SPMatrix::MinValue() {
    F32vec4 min1 = _mm_min_ps(_mm_abs_ps(_L1), _mm_abs_ps(_L2));
    F32vec4 min2 = _mm_min_ps(_mm_abs_ps(_L3), _mm_abs_ps(_L4));
    F32vec4 min = _mm_min_ps(min1, min2);
    min = _mm_min_ps(min, _mm_movehl_ps(min,min));
    min = _mm_min_ss(min, _mm_shuffle_ps(min,min,0x01));
    return min[0];
}

// ----------------------------------------------------------------------------
//  Name:   SPMatrix::MaxValue
//  Desc:   Returns the asbolute maximum element of the matrix.
// ----------------------------------------------------------------------------
float SPMatrix::MaxValue() {
    F32vec4 max1 = _mm_max_ps(_mm_abs_ps(_L1), _mm_abs_ps(_L2));
    F32vec4 max2 = _mm_max_ps(_mm_abs_ps(_L3), _mm_abs_ps(_L4));
    F32vec4 max = _mm_max_ps(max1, max2);
    max = _mm_max_ps(max, _mm_movehl_ps(max,max));
    max = _mm_max_ss(max, _mm_shuffle_ps(max,max,0x01));
    return max[0];
}

// ----------------------------------------------------------------------------
//  Name:   SPMatrix::Determinant
//  Desc:   Return the matrix determinant. A = det[M].
// ----------------------------------------------------------------------------
float SPMatrix::Determinant() {
    __m128 Va,Vb,Vc;
    __m128 r1,r2,r3,t1,t2,sum;
    F32vec4 Det;

    // First, Let's calculate the first four minterms of the first line
    t1 = _L4; t2 = _mm_ror_ps(_L3,1); 
    Vc = _mm_mul_ps(t2,_mm_ror_ps(t1,0));                   // V3'·V4
    Va = _mm_mul_ps(t2,_mm_ror_ps(t1,2));                   // V3'·V4"
    Vb = _mm_mul_ps(t2,_mm_ror_ps(t1,3));                   // V3'·V4^

    r1 = _mm_sub_ps(_mm_ror_ps(Va,1),_mm_ror_ps(Vc,2));     // V3"·V4^ - V3^·V4"
    r2 = _mm_sub_ps(_mm_ror_ps(Vb,2),_mm_ror_ps(Vb,0));     // V3^·V4' - V3'·V4^
    r3 = _mm_sub_ps(_mm_ror_ps(Va,0),_mm_ror_ps(Vc,1));     // V3'·V4" - V3"·V4'

    Va = _mm_ror_ps(_L2,1);     sum = _mm_mul_ps(Va,r1);
    Vb = _mm_ror_ps(Va,1);      sum = _mm_add_ps(sum,_mm_mul_ps(Vb,r2));
    Vc = _mm_ror_ps(Vb,1);      sum = _mm_add_ps(sum,_mm_mul_ps(Vc,r3));

    // Now we can calculate the determinant:
    Det = _mm_mul_ps(sum,_L1);
    Det = _mm_add_ps(Det,_mm_movehl_ps(Det,Det));
    Det = _mm_sub_ss(Det,_mm_shuffle_ps(Det,Det,1));
    return Det[0];
}

// ----------------------------------------------------------------------------
//  Name:   SPMatrix::Inverse
//  Desc:   Inverse the 4x4 matrix. Matrix is set to inv[M].
//  Note:   In case of non-inversable matrix, sets the matrix to a Zero matrix (depending on the switch).
// ----------------------------------------------------------------------------
float SPMatrix::Inverse()
{
    // The inverse is calculated using "Divide and Conquer" technique. The 
    // original matrix is divide into four 2x2 sub-matrices. Since each 
    // register holds four matrix element, the smaller matrices are 
    // represented as a registers. Hence we get a better locality of the 
    // calculations.

    F32vec4 A = _mm_movelh_ps(_L1, _L2),    // the four sub-matrices 
            B = _mm_movehl_ps(_L2, _L1),
            C = _mm_movelh_ps(_L3, _L4),
            D = _mm_movehl_ps(_L4, _L3);
    F32vec4 iA, iB, iC, iD,					// partial inverse of the sub-matrices
            DC, AB;
    F32vec1 dA, dB, dC, dD;                 // determinant of the sub-matrices
    F32vec1 det, d, d1, d2;
    F32vec4 rd;

    //  AB = A# * B
    AB = _mm_mul_ps(_mm_shuffle_ps(A,A,0x0F), B);
    AB -= (F32vec4)_mm_mul_ps(_mm_shuffle_ps(A,A,0xA5), _mm_shuffle_ps(B,B,0x4E));
    //  DC = D# * C
    DC = _mm_mul_ps(_mm_shuffle_ps(D,D,0x0F), C);
    DC -= (F32vec4)_mm_mul_ps(_mm_shuffle_ps(D,D,0xA5), _mm_shuffle_ps(C,C,0x4E));

    //  dA = |A|
    dA = _mm_mul_ps(_mm_shuffle_ps(A, A, 0x5F),A);
    dA = _mm_sub_ss(dA, _mm_movehl_ps(dA,dA));
    //  dB = |B|
    dB = _mm_mul_ps(_mm_shuffle_ps(B, B, 0x5F),B);
    dB = _mm_sub_ss(dB, _mm_movehl_ps(dB,dB));

    //  dC = |C|
    dC = _mm_mul_ps(_mm_shuffle_ps(C, C, 0x5F),C);
    dC = _mm_sub_ss(dC, _mm_movehl_ps(dC,dC));
    //  dD = |D|
    dD = _mm_mul_ps(_mm_shuffle_ps(D, D, 0x5F),D);
    dD = _mm_sub_ss(dD, _mm_movehl_ps(dD,dD));

    //  d = trace(AB*DC) = trace(A#*B*D#*C)
    d = _mm_mul_ps(_mm_shuffle_ps(DC,DC,0xD8),AB);

    //  iD = C*A#*B
    iD = _mm_mul_ps(_mm_shuffle_ps(C,C,0xA0), _mm_movelh_ps(AB,AB));
    iD += (F32vec4)_mm_mul_ps(_mm_shuffle_ps(C,C,0xF5), _mm_movehl_ps(AB,AB));
    //  iA = B*D#*C
    iA = _mm_mul_ps(_mm_shuffle_ps(B,B,0xA0), _mm_movelh_ps(DC,DC));
    iA += (F32vec4)_mm_mul_ps(_mm_shuffle_ps(B,B,0xF5), _mm_movehl_ps(DC,DC));

    //  d = trace(AB*DC) = trace(A#*B*D#*C) [continue]
    d = _mm_add_ps(d, _mm_movehl_ps(d, d));
    d = _mm_add_ss(d, _mm_shuffle_ps(d, d, 1));
    d1 = dA*dD;
    d2 = dB*dC;

    //  iD = D*|A| - C*A#*B
    iD = D*_mm_shuffle_ps(dA,dA,0) - iD;

    //  iA = A*|D| - B*D#*C;
    iA = A*_mm_shuffle_ps(dD,dD,0) - iA;

    //  det = |A|*|D| + |B|*|C| - trace(A#*B*D#*C)
    det = d1+d2-d;
    rd = (__m128)(F32vec1(1.0f)/det);
    #ifdef ZERO_SINGULAR
        rd = _mm_and_ps(_mm_cmpneq_ss(det,_mm_setzero_ps()), rd);
    #endif

    //  iB = D * (A#B)# = D*B#*A
    iB = _mm_mul_ps(D, _mm_shuffle_ps(AB,AB,0x33));
    iB -= (F32vec4)_mm_mul_ps(_mm_shuffle_ps(D,D,0xB1), _mm_shuffle_ps(AB,AB,0x66));
    //  iC = A * (D#C)# = A*C#*D
    iC = _mm_mul_ps(A, _mm_shuffle_ps(DC,DC,0x33));
    iC -= (F32vec4)_mm_mul_ps(_mm_shuffle_ps(A,A,0xB1), _mm_shuffle_ps(DC,DC,0x66));

    rd = _mm_shuffle_ps(rd,rd,0);
    rd ^= Sign_PNNP;

    //  iB = C*|B| - D*B#*A
    iB = C*_mm_shuffle_ps(dB,dB,0) - iB;

    //  iC = B*|C| - A*C#*D;
    iC = B*_mm_shuffle_ps(dC,dC,0) - iC;

    //  iX = iX / det
    iA *= rd;
    iB *= rd;
    iC *= rd;
    iD *= rd;

    _L1 = _mm_shuffle_ps(iA,iB,0x77);
    _L2 = _mm_shuffle_ps(iA,iB,0x22);
    _L3 = _mm_shuffle_ps(iC,iD,0x77);
    _L4 = _mm_shuffle_ps(iC,iD,0x22);

    return *(float*)&det;
}

