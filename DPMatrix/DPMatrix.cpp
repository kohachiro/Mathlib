//////////////////////////////////////////////////////////////////////////////
//
// DPMATRIX.CPP
//
//  Implementation of the DPMatrix, DPVector and the DPVector3 classes.
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


#include "DPMatrix.h"
#include <memory.h>


const _MM_ALIGN16 __int64 __MASKSIGNd_[2] = { 0x8000000000000000, 0x8000000000000000 };
const _MM_ALIGN16 __int64 _Sign_NP[2] = { 0x8000000000000000, 0x0000000000000000 };
const _MM_ALIGN16 __int64 _Sign_PN[2] = { 0x0000000000000000, 0x8000000000000000 };
const _MM_ALIGN16 double __ZERONE_[2] = { 0.0, 1.0 };

#define _MASKSIGNd_ (*(F64vec2*)&__MASKSIGNd_)
#define Sign_NP     (*(F64vec2*)&_Sign_NP)
#define Sign_PN     (*(F64vec2*)&_Sign_PN)
#define _ZERONE_    (*(F64vec2*)&__ZERONE_)

const double one = 1.0;


            /***********************************************************/
            /*                Matrix Constructors                      */
            /***********************************************************/

// ----------------------------------------------------------------------------
//  Name:   RotateXMatrix
//  Desc:   Returns the rotation matrix of X axis of specific angle.
// ----------------------------------------------------------------------------
void DPMatrix::RotateXMatrix(const double rads) {
    __asm { 
        fld     double ptr rads
        fsincos
        xorpd   xmm0,xmm0
        mov     eax, this
        movsd   xmm1, one
        movsd   [eax+0x20], xmm0        // _21
        movsd   [eax+0x38], xmm0        // _24
        movsd   [eax+0x40], xmm0        // _31
        movsd   [eax+0x58], xmm0        // _34
        movapd  [eax+0x00], xmm1        // _11, _12
        movapd  [eax+0x10], xmm0        // _13, _14
        shufpd  xmm1,xmm1,1
        movapd  [eax+0x60], xmm0        // _41, _42
        movapd  [eax+0x70], xmm1        // _43, _44

        fst     double ptr [eax+0x28]   // _22
        fstp    double ptr [eax+0x50]   // _33
        fst     double ptr [eax+0x30]   // _23
        fchs
        fstp    double ptr [eax+0x48]   // _32
    }
} 
DPMatrix RotateXMatrix(const double rads) {
    DPMatrix ret;
    ret.RotateXMatrix(rads);
    return ret;
} 

// ----------------------------------------------------------------------------
//  Name:   RotateYMatrix
//  Desc:   Returns the rotation matrix of Y axis of specific angle.
// ----------------------------------------------------------------------------
void DPMatrix::RotateYMatrix(const double rads) {
    __asm { 
        fld     double ptr rads
        fsincos
        xorpd   xmm0,xmm0
        mov     eax, this
        movapd  xmm1, __ZERONE_
        movsd   [eax+0x08], xmm0        // _12
        movsd   [eax+0x18], xmm0        // _14
        movsd   [eax+0x48], xmm0        // _32
        movsd   [eax+0x58], xmm0        // _34
        movapd  [eax+0x20], xmm1        // _21, _22
        movapd  [eax+0x30], xmm0        // _23, _24
        movapd  [eax+0x60], xmm0        // _41, _42
        movapd  [eax+0x70], xmm1        // _43, _44

        fst     double ptr [eax+0x00]   // _11
        fstp    double ptr [eax+0x50]   // _33
        fst     double ptr [eax+0x40]   // _31
        fchs
        fstp    double ptr [eax+0x10]   // _13
    }
} 
DPMatrix RotateYMatrix(const double rads) {
    DPMatrix ret;
    ret.RotateYMatrix(rads);
    return ret;
} 

// ----------------------------------------------------------------------------
//  Name:   RotateZMatrix
//  Desc:   Returns the rotation matrix of Z axis of specific angle.
// ----------------------------------------------------------------------------
void DPMatrix::RotateZMatrix(const double rads) {
    __asm { 
        fld     double ptr rads
        fsincos
        xorpd   xmm0,xmm0
        mov     eax, this
        movsd   xmm1, one
        movapd  [eax+0x10], xmm0        // _13, _14
        movapd  [eax+0x30], xmm0        // _23, _24
        movapd  [eax+0x40], xmm0        // _31, _32
        movapd  [eax+0x50], xmm1        // _33, _34
        shufpd  xmm1,xmm1,1
        movapd  [eax+0x60], xmm0        // _41, _42
        movapd  [eax+0x70], xmm1        // _43, _44

        fst     double ptr [eax+0x00]   // _11
        fstp    double ptr [eax+0x28]   // _22
        fst     double ptr [eax+0x08]   // _12
        fchs
        fstp    double ptr [eax+0x20]   // _21
    }
} 
DPMatrix RotateZMatrix(const double rads) {
    DPMatrix ret;
    ret.RotateZMatrix(rads);
    return ret;
} 

// ----------------------------------------------------------------------------
//  Name:   TranslateMatrix
//  Desc:   Returns the translation matrix for specific translation.
// ----------------------------------------------------------------------------
DPMatrix TranslateMatrix(const double dx, const double dy, const double dz)
{
    DPMatrix ret;
    ret.IdentityMatrix();
    ret._L4a = F64vec2(dy,dx);
    ret._L4b = F64vec2(1.0,dz);
    return ret;
} 
void DPMatrix::TranslateMatrix(const double dx, const double dy, const double dz)
{
    IdentityMatrix();
    _L4a = F64vec2(dy,dx);
    _L4b = F64vec2(1.0,dz);
} 

// ----------------------------------------------------------------------------
//  Name:   ScaleMatrix
//  Desc:   Returns the scaling matrix for x,y,z axis.
// ----------------------------------------------------------------------------
DPMatrix ScaleMatrix(const double a, const double b, const double c)
{
    DPMatrix ret;
    ret.IdentityMatrix();
    ret._L1a = _mm_set_sd(a);
    ret._L2a = _mm_set_sd(b);
    ret._L2a = _mm_shuffle_pd(ret._L2a,ret._L2a,1);
    ret._L3b = _mm_set_sd(c);
    return ret;
} 
void DPMatrix::ScaleMatrix(const double a, const double b, const double c)
{
    IdentityMatrix();
    _L1a = _mm_set_sd(a);
    _L2a = _mm_set_sd(b);
    _L2a = _mm_shuffle_pd(_L2a,_L2a,1);
    _L3b = _mm_set_sd(c);
} 

// ----------------------------------------------------------------------------
//  Name:   ScaleMatrix
//  Desc:   Returns the scaling matrix by a scalar.
// ----------------------------------------------------------------------------
DPMatrix ScaleMatrix(const double a)
{
    DPMatrix ret;
    ret.IdentityMatrix();
    ret._L1a = _mm_set_sd(a);
    ret._L2a = _mm_shuffle_pd(ret._L1a,ret._L1a,1);
    ret._L3b = ret._L1a;
    return ret;
} 
void DPMatrix::ScaleMatrix(const double a)
{
    IdentityMatrix();
    _L1a = _mm_set_sd(a);
    _L2a = _mm_shuffle_pd(_L1a,_L1a,1);
    _L3b = _L1a;
} 


            /***********************************************************/
            /*                    Matrix Operators                     */
            /***********************************************************/

// ----------------------------------------------------------------------------
//  Name:   DPMatrix::MinValue
//  Desc:   Returns the asbolute minimum element of the matrix.
// ----------------------------------------------------------------------------
double DPMatrix::MinValue() {
    __m128d min1 = _mm_min_pd(_mm_abs_pd(_L1a), _mm_abs_pd(_L1b));
    __m128d min2 = _mm_min_pd(_mm_abs_pd(_L2a), _mm_abs_pd(_L2b));
    __m128d min3 = _mm_min_pd(_mm_abs_pd(_L3a), _mm_abs_pd(_L3b));
    __m128d min4 = _mm_min_pd(_mm_abs_pd(_L4a), _mm_abs_pd(_L4b));
    min1 = _mm_min_pd(min1, min2);
    min3 = _mm_min_pd(min3, min4);
    __m128d min = _mm_min_pd(min1, min3);
    min = _mm_min_sd(min, _mm_unpackhi_pd(min,min));
    return *(double*)&min;
}

// ----------------------------------------------------------------------------
//  Name:   DPMatrix::MaxValue
//  Desc:   Returns the asbolute maximum element of the matrix.
// ----------------------------------------------------------------------------
double DPMatrix::MaxValue() {
    __m128d max1 = _mm_max_pd(_mm_abs_pd(_L1a), _mm_abs_pd(_L1b));
    __m128d max2 = _mm_max_pd(_mm_abs_pd(_L2a), _mm_abs_pd(_L2b));
    __m128d max3 = _mm_max_pd(_mm_abs_pd(_L3a), _mm_abs_pd(_L3b));
    __m128d max4 = _mm_max_pd(_mm_abs_pd(_L4a), _mm_abs_pd(_L4b));
    max1 = _mm_max_pd(max1, max2);
    max3 = _mm_max_pd(max3, max4);
    __m128d max = _mm_max_pd(max1, max3);
    max = _mm_max_sd(max, _mm_unpackhi_pd(max,max));
    return *(double*)&max;
}

// ----------------------------------------------------------------------------
//  Name:   DPMatrix::Determinant
//  Desc:   Return the matrix determinant. A = det[M].
// ----------------------------------------------------------------------------

#define LOADSD(x) _mm_shuffle_pd(_mm_set_sd(x),_mm_set_sd(x),0)

double DPMatrix::Determinant() {
    F64vec4 AB, DC;
    F64vec2 dA, dB, dC, dD;
    F64vec2 d, d1, d2;

    //  AB = A# * B;
    AB.vec1 = _mm_mul_pd(_L1b, LOADSD(_22));
    AB.vec2 = _mm_mul_pd(_L2b, LOADSD(_11));
    AB.vec1 = _mm_sub_pd(AB.vec1, _mm_mul_pd(_L2b, LOADSD(_12)));
    AB.vec2 = _mm_sub_pd(AB.vec2, _mm_mul_pd(_L1b, LOADSD(_21)));

    //  dA = |A|;
    //  dB = |B|;
    dA = _mm_shuffle_pd(_L2a, _L2a, 1);
    dB = _mm_shuffle_pd(_L2b, _L2b, 1);
    dA = _mm_mul_pd(_L1a,dA);
    dB = _mm_mul_pd(_L1b,dB);
    dA = _mm_sub_sd(dA, _mm_shuffle_pd(dA,dA,3));
    dB = _mm_sub_sd(dB, _mm_shuffle_pd(dB,dB,3));

    //  DC = D# * C;
    DC.vec1 = _mm_mul_pd(_L3a, LOADSD(_44));
    DC.vec2 = _mm_mul_pd(_L4a, LOADSD(_33));
    DC.vec1 = _mm_sub_pd(DC.vec1, _mm_mul_pd(_L4a, LOADSD(_34)));
    DC.vec2 = _mm_sub_pd(DC.vec2, _mm_mul_pd(_L3a, LOADSD(_43)));

    //  dD = |D|;
    //  dC = |C|;
    dD = _mm_shuffle_pd(_L4b, _L4b, 1);
    dC = _mm_shuffle_pd(_L4a, _L4a, 1);
    dD = _mm_mul_pd(_L3b,dD);
    dC = _mm_mul_pd(_L3a,dC);
    dD = _mm_sub_sd(dD, _mm_shuffle_pd(dD,dD,3));
    dC = _mm_sub_sd(dC, _mm_shuffle_pd(dC,dC,3));

    //  d = trace(AB*DC) = trace(A#*B*D#*C)
    d1 = _mm_mul_pd(AB.vec1, _mm_shuffle_pd(DC.vec1, DC.vec2, 0));
    d2 = _mm_mul_pd(AB.vec2, _mm_shuffle_pd(DC.vec1, DC.vec2, 3));
    d1 = _mm_add_pd(d1, d2);
    d = _mm_add_sd(d1, _mm_shuffle_pd(d1, d1,3));

    d1 = _mm_mul_sd(dA, dD);    // |A|*|D|
    d2 = _mm_mul_sd(dB, dC);    // |B|*|C|
    d1 = _mm_add_sd(d1, d2);    // |A|*|D| + |B|*|C|
    d1 = _mm_sub_sd(d1, d);
    return d1[0];
}

// ----------------------------------------------------------------------------
//  Name:   DPMatrix::Inverse
//  Desc:   Inverse the 4x4 matrix. Matrix is set to inv[M].
//  Note:   In case of non-inversable matrix, sets the matrix to a Zero matrix (depending on the switch).
// ----------------------------------------------------------------------------
double DPMatrix::Inverse()
{
    // The inverse is calculated using "Divide and Conquer" technique. The 
    // original matrix is divide into four 2x2 sub-matrices. Since each 
    // register of the matrix holds two element, the smaller matrices are 
    // consisted of two registers. Hence we get a better locality of the 
    // calculations.

    F64vec4 A(_L2a, _L1a),      // the four sub-matrices
            B(_L2b, _L1b),
            C(_L4a, _L3a),
            D(_L4b, _L3b);
    F64vec4 iA, iB, iC, iD,     // partial invese of the sub-matrices
            DC, AB;
    F64vec2 dA, dB, dC, dD;     // determinant of the sub-matrices
    F64vec2 det, d1, d2, rd;

    //  dA = |A|
    dA = _mm_shuffle_pd(A.vec2, A.vec2, 1);
    dA = _mm_mul_pd(A.vec1, dA);
    dA = _mm_sub_sd(dA, _mm_shuffle_pd(dA,dA,3));
    //  dB = |B|
    dB = _mm_shuffle_pd(B.vec2, B.vec2, 1);
    dB = _mm_mul_pd(B.vec1, dB);
    dB = _mm_sub_sd(dB, _mm_shuffle_pd(dB,dB,3));

    //  AB = A# * B
    AB.vec1 = _mm_mul_pd(B.vec1, _mm_shuffle_pd(A.vec2,A.vec2,3));
    AB.vec2 = _mm_mul_pd(B.vec2, _mm_shuffle_pd(A.vec1,A.vec1,0));
    AB.vec1 = _mm_sub_pd(AB.vec1, _mm_mul_pd(B.vec2, _mm_shuffle_pd(A.vec1,A.vec1,3)));
    AB.vec2 = _mm_sub_pd(AB.vec2, _mm_mul_pd(B.vec1, _mm_shuffle_pd(A.vec2,A.vec2,0)));

    //  dC = |C|
    dC = _mm_shuffle_pd(C.vec2, C.vec2, 1);
    dC = _mm_mul_pd(C.vec1, dC);
    dC = _mm_sub_sd(dC, _mm_shuffle_pd(dC,dC,3));
    //  dD = |D|
    dD = _mm_shuffle_pd(D.vec2, D.vec2, 1);
    dD = _mm_mul_pd(D.vec1, dD);
    dD = _mm_sub_sd(dD, _mm_shuffle_pd(dD,dD,3));

    //  DC = D# * C
    DC.vec1 = _mm_mul_pd(C.vec1, _mm_shuffle_pd(D.vec2,D.vec2,3));
    DC.vec2 = _mm_mul_pd(C.vec2, _mm_shuffle_pd(D.vec1,D.vec1,0));
    DC.vec1 = _mm_sub_pd(DC.vec1, _mm_mul_pd(C.vec2, _mm_shuffle_pd(D.vec1,D.vec1,3)));
    DC.vec2 = _mm_sub_pd(DC.vec2, _mm_mul_pd(C.vec1, _mm_shuffle_pd(D.vec2,D.vec2,0)));

    //  rd = trace(AB*DC) = trace(A#*B*D#*C)
    d1 = _mm_mul_pd(AB.vec1, _mm_shuffle_pd(DC.vec1, DC.vec2, 0));
    d2 = _mm_mul_pd(AB.vec2, _mm_shuffle_pd(DC.vec1, DC.vec2, 3));
    rd = _mm_add_pd(d1, d2);
    rd = _mm_add_sd(rd, _mm_shuffle_pd(rd, rd,3));

    //  iD = C*A#*B
    iD.vec1 = _mm_mul_pd(AB.vec1, _mm_shuffle_pd(C.vec1,C.vec1,0));
    iD.vec2 = _mm_mul_pd(AB.vec1, _mm_shuffle_pd(C.vec2,C.vec2,0));
    iD.vec1 = _mm_add_pd(iD.vec1, _mm_mul_pd(AB.vec2, _mm_shuffle_pd(C.vec1,C.vec1,3)));
    iD.vec2 = _mm_add_pd(iD.vec2, _mm_mul_pd(AB.vec2, _mm_shuffle_pd(C.vec2,C.vec2,3)));

    //  iA = B*D#*C
    iA.vec1 = _mm_mul_pd(DC.vec1, _mm_shuffle_pd(B.vec1,B.vec1,0));
    iA.vec2 = _mm_mul_pd(DC.vec1, _mm_shuffle_pd(B.vec2,B.vec2,0));
    iA.vec1 = _mm_add_pd(iA.vec1, _mm_mul_pd(DC.vec2, _mm_shuffle_pd(B.vec1,B.vec1,3)));
    iA.vec2 = _mm_add_pd(iA.vec2, _mm_mul_pd(DC.vec2, _mm_shuffle_pd(B.vec2,B.vec2,3)));
    
    //  iD = D*|A| - C*A#*B
    dA = _mm_shuffle_pd(dA,dA,0);
    iD.vec1 = _mm_sub_pd(_mm_mul_pd(D.vec1, dA), iD.vec1);
    iD.vec2 = _mm_sub_pd(_mm_mul_pd(D.vec2, dA), iD.vec2);

    //  iA = A*|D| - B*D#*C;
    dD = _mm_shuffle_pd(dD,dD,0);
    iA.vec1 = _mm_sub_pd(_mm_mul_pd(A.vec1, dD), iA.vec1);
    iA.vec2 = _mm_sub_pd(_mm_mul_pd(A.vec2, dD), iA.vec2);
    
    d1 = _mm_mul_sd(dA, dD);
    d2 = _mm_mul_sd(dB, dC);
    
    //  iB = D * (A#B)# = D*B#*A
    iB.vec1 = _mm_mul_pd(D.vec1, _mm_shuffle_pd(AB.vec2,AB.vec1,1));
    iB.vec2 = _mm_mul_pd(D.vec2, _mm_shuffle_pd(AB.vec2,AB.vec1,1));
    iB.vec1 = _mm_sub_pd(iB.vec1, _mm_mul_pd(_mm_shuffle_pd(D.vec1,D.vec1,1), _mm_shuffle_pd(AB.vec2,AB.vec1,2)));
    iB.vec2 = _mm_sub_pd(iB.vec2, _mm_mul_pd(_mm_shuffle_pd(D.vec2,D.vec2,1), _mm_shuffle_pd(AB.vec2,AB.vec1,2)));

    //  det = |A|*|D| + |B|*|C| - trace(A#*B*D#*C)
    det = _mm_add_sd(d1, d2);
    det = _mm_sub_sd(det, rd);

    //  iC = A * (D#C)# = A*C#*D
    iC.vec1 = _mm_mul_pd(A.vec1, _mm_shuffle_pd(DC.vec2,DC.vec1,1));
    iC.vec2 = _mm_mul_pd(A.vec2, _mm_shuffle_pd(DC.vec2,DC.vec1,1));
    iC.vec1 = _mm_sub_pd(iC.vec1, _mm_mul_pd(_mm_shuffle_pd(A.vec1,A.vec1,1), _mm_shuffle_pd(DC.vec2,DC.vec1,2)));
    iC.vec2 = _mm_sub_pd(iC.vec2, _mm_mul_pd(_mm_shuffle_pd(A.vec2,A.vec2,1), _mm_shuffle_pd(DC.vec2,DC.vec1,2)));

    rd = _mm_div_sd(_mm_set_sd(1.0), det);
    #ifdef ZERO_SINGULAR
        rd = _mm_and_pd(_mm_cmpneq_sd(det,_mm_setzero_pd()), rd);
    #endif
    rd = _mm_shuffle_pd(rd,rd,0);

    //  iB = C*|B| - D*B#*A
    dB = _mm_shuffle_pd(dB,dB,0);
    iB.vec1 = _mm_sub_pd(_mm_mul_pd(C.vec1, dB), iB.vec1);
    iB.vec2 = _mm_sub_pd(_mm_mul_pd(C.vec2, dB), iB.vec2);

    d1 = _mm_xor_pd(rd, Sign_PN);
    d2 = _mm_xor_pd(rd, Sign_NP);

    //  iC = B*|C| - A*C#*D;
    dC = _mm_shuffle_pd(dC,dC,0);
    iC.vec1 = _mm_sub_pd(_mm_mul_pd(B.vec1, dC), iC.vec1);
    iC.vec2 = _mm_sub_pd(_mm_mul_pd(B.vec2, dC), iC.vec2);

    _L1a = _mm_mul_pd(_mm_shuffle_pd(iA.vec2, iA.vec1, 3), d1);     // iA# / det
    _L2a = _mm_mul_pd(_mm_shuffle_pd(iA.vec2, iA.vec1, 0), d2);
    _L1b = _mm_mul_pd(_mm_shuffle_pd(iB.vec2, iB.vec1, 3), d1);     // iB# / det
    _L2b = _mm_mul_pd(_mm_shuffle_pd(iB.vec2, iB.vec1, 0), d2);
    _L3a = _mm_mul_pd(_mm_shuffle_pd(iC.vec2, iC.vec1, 3), d1);     // iC# / det
    _L4a = _mm_mul_pd(_mm_shuffle_pd(iC.vec2, iC.vec1, 0), d2);
    _L3b = _mm_mul_pd(_mm_shuffle_pd(iD.vec2, iD.vec1, 3), d1);     // iD# / det
    _L4b = _mm_mul_pd(_mm_shuffle_pd(iD.vec2, iD.vec1, 0), d2);

    return det[0];
}

